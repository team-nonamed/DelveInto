#include "Skills/Instances/SkillInstance.h"
#include "Skills/Data/SkillData.h"

void USkillInstance::Init(USkillData* InData, int32 InLevel)
{
	Data = InData;
	Level = InLevel;

	// 런타임 상태 초기화
	CooldownEndTime = 0.f;
	ComboIndex = 0;
	ComboExpireTime = 0.f;
	bCharging = false;
	ChargeStartTime = 0.f;
	CachedChargeAlpha = 0.f;
}

bool USkillInstance::IsOnCooldown(const UWorld* World) const
{
	if (!World) return false;
	return World->GetTimeSeconds() < CooldownEndTime;
}

float USkillInstance::GetRemainingCooldown(const UWorld* World) const
{
	if (!World) return 0.f;
	return FMath::Max(0.f, CooldownEndTime - World->GetTimeSeconds());
}

bool USkillInstance::OnInputStarted(const UWorld* World, const FSkillContext& /*Ctx*/)
{
	if (!World || !Data) return false;

	const FSkillExecSpec& Exec = Data->GetExec();

	// HoldRelease: 차징 시작
	if (Exec.CastType == ESkillCastType::HoldRelease)
	{
		bCharging = true;
		ChargeStartTime = World->GetTimeSeconds();
		CachedChargeAlpha = 0.f;
		return true;
	}

	// Tap: 콤보 갱신(ComboDamages가 있을 때만)
	if (Exec.Damage.ComboDamages.Num() > 0)
	{
		const float Now = World->GetTimeSeconds();

		// 콤보 윈도우 밖이면 리셋
		if (Now > ComboExpireTime)
		{
			ComboIndex = 0;
		}

		ComboExpireTime = Now + Exec.ComboWindowSeconds;
		// Tap은 “Started에 대한 상태만 갱신”하고, 실제 실행은 AttackHandler가 CanActivate 후 진행
		return true;
	}

	return true;
}

bool USkillInstance::OnInputCompleted(const UWorld* World, const FSkillContext& /*Ctx*/)
{
	if (!World || !Data) return false;

	// HoldRelease: 차징 종료(발동 후보)
	const FSkillExecSpec& Exec = Data->GetExec();
	if (Exec.CastType == ESkillCastType::HoldRelease)
	{
		if (bCharging)
		{
			const float Now = World->GetTimeSeconds();
			const float Elapsed = FMath::Max(0.f, Now - ChargeStartTime);
			const float Denom = FMath::Max(KINDA_SMALL_NUMBER, Exec.Damage.MaxChargeTime);
			CachedChargeAlpha = FMath::Clamp(Elapsed / Denom, 0.f, 1.f);
		}
		bCharging = false;
		return true;
	}

	return true;
}

bool USkillInstance::OnInputCanceled(const UWorld* /*World*/, const FSkillContext& /*Ctx*/)
{
	ResetCharge();
	return true;
}

bool USkillInstance::CanActivate(const UWorld* World, const FSkillContext& /*Ctx*/) const
{
	if (!World || !Data) return false;

	// 1) 쿨다운 체크
	if (IsOnCooldown(World))
	{
		return false;
	}

	// 2) HoldRelease는 “Completed 이후에만” 실제 발동 가능하도록 정책을 둘 수 있음
	const FSkillExecSpec& Exec = Data->GetExec();
	if (Exec.CastType == ESkillCastType::HoldRelease)
	{
		// 차징 중이면 아직 발동 불가(Started 상태)
		if (bCharging)
		{
			return false;
		}
	}

	return true;
}

int32 USkillInstance::ResolveComboIndex(const UWorld* World) const
{
	if (!World || !Data) return 0;

	const FSkillExecSpec& Exec = Data->GetExec();
	if (Exec.Damage.ComboDamages.Num() == 0) return 0;

	// 콤보 윈도우 만료 시 0으로 간주
	if (World->GetTimeSeconds() > ComboExpireTime)
	{
		return 0;
	}
	return ComboIndex;
}

float USkillInstance::ResolveChargeAlpha(const UWorld* World) const
{
	if (!World || !Data) return 0.f;

	const FSkillExecSpec& Exec = Data->GetExec();
	if (Exec.CastType != ESkillCastType::HoldRelease) return 0.f;

	// Completed 이후 CachedChargeAlpha 사용
	return CachedChargeAlpha;
}

float USkillInstance::ResolveDamage(const UWorld* World) const
{
	if (!World || !Data) return 0.f;

	const FSkillExecSpec& Exec = Data->GetExec();
	const FDamageSpec& Dmg = Exec.Damage;

	float Damage = 0.f;

	// 1) 콤보
	if (Dmg.ComboDamages.Num() > 0)
	{
		const int32 Idx = ResolveComboIndex(World);
		const int32 SafeIdx = FMath::Clamp(Idx, 0, Dmg.ComboDamages.Num() - 1);
		Damage = Dmg.ComboDamages[SafeIdx];
	}
	// 2) 차징
	else if (Exec.CastType == ESkillCastType::HoldRelease && Dmg.MaxChargeTime > 0.f)
	{
		const float Alpha = ResolveChargeAlpha(World);
		Damage = FMath::Lerp(Dmg.MinChargedDamage, Dmg.MaxChargedDamage, Alpha);
	}
	// 3) 단발
	else
	{
		Damage = Dmg.BaseDamage;
	}

	// 4) 기존 설계 호환: Additive / MultiplierAdditive를 반영(선택 정책)
	Damage = (Damage + Dmg.Additive) * (1.f + Dmg.MultiplierAdditive);

	return Damage;
}

void USkillInstance::CommitActivate(const UWorld* World)
{
	if (!World || !Data) return;

	const FSkillExecSpec& Exec = Data->GetExec();

	// 쿨다운 확정
	CooldownEndTime = World->GetTimeSeconds() + Exec.CooldownSeconds;

	// 콤보 인덱스 증가(ComboDamages가 있는 경우)
	if (Exec.Damage.ComboDamages.Num() > 0)
	{
		ComboIndex = (ComboIndex + 1) % Exec.Damage.ComboDamages.Num();
		ComboExpireTime = World->GetTimeSeconds() + Exec.ComboWindowSeconds;
	}

	// 차징 상태 정리
	ResetCharge();
}

void USkillInstance::ResetCombo()
{
	ComboIndex = 0;
	ComboExpireTime = 0.f;
}

void USkillInstance::ResetCharge()
{
	bCharging = false;
	ChargeStartTime = 0.f;
	CachedChargeAlpha = 0.f;
}
