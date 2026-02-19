#include "BaseAttack.h"
#include "NewSystem/Utils/CombatUtil.h" // 우리가 만든 정적 유틸리티 함수
#include "Kismet/GameplayStatics.h"

USkill_LongSword_BaseAttack::USkill_LongSword_BaseAttack()
{
	bHasCombo = true;
	MaxComboCount = 3;
	ComboTerminationType = EComboTerminationType::Loop;
}

float USkill_LongSword_BaseAttack::CalculateDamage() const
{
	switch (CurrentComboCount)
	{
	case 0:
		return Super::CalculateDamage();
	case 1:
		return SecondComboDamage;
	case 2:
		return ThirdComboDamage;
	default:
		UE_LOG(LogSkill, Error, TEXT("일반적으로 접근할 수 없는 Combo입니다."))
		return 0.0f;
	}
}

bool USkill_LongSword_BaseAttack::ExecuteSkillLogic_Implementation()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return false;

	// 1. 데미지 계산
	float Damage = CalculateDamage();

	// 2. 공격 판정 (사거리 400.0f, 각도 15.0f)
	// CombatUtil을 사용하여 깔끔하게 처리
	bool bHit = UCombatUtil::ApplyDamageSphericalCone(this, OwnerActor, Damage, 400.0f, 15.0f, OwnerActor);

	// 3. 사운드 재생
	if (USoundBase* SoundToPlay = bHit ? CastButHurtSuccessSound : CastButHurtFailedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, OwnerActor->GetActorLocation());
	}

	// 4. 공격 성공 여부 반환
	return bHit;
}
