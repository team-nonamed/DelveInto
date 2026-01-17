#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Types/SkillTypes.h"
#include "SkillInstance.generated.h"

class USkillData;

/**
 * 런타임 상태를 가진 Skill 인스턴스.
 * - 쿨다운/콤보/차징 같은 “상태”만 담당.
 * - 실제 월드 액션(타겟팅/투사체 스폰/피해 적용)은 AttackHandler가 담당.
 */
UCLASS(BlueprintType)
class DELVEINTO_API USkillInstance : public UObject
{
	GENERATED_BODY()

public:
	void Init(USkillData* InData, int32 InLevel = 1);

	const USkillData* GetData() const { return Data; }
	int32 GetLevel() const { return Level; }

	// ----- 쿨다운 -----
	bool IsOnCooldown(const UWorld* World) const;
	float GetRemainingCooldown(const UWorld* World) const;

	// ----- 입력 이벤트(상태 전이) -----
	bool OnInputStarted(const UWorld* World, const FSkillContext& Ctx);
	bool OnInputCompleted(const UWorld* World, const FSkillContext& Ctx);
	bool OnInputCanceled(const UWorld* World, const FSkillContext& Ctx);

	// ----- 실행 가능 여부 / 실행 파라미터 -----
	bool CanActivate(const UWorld* World, const FSkillContext& Ctx) const;

	float ResolveDamage(const UWorld* World) const;       // 콤보/차징 상태를 반영해 실제 데미지 산출
	int32 ResolveComboIndex(const UWorld* World) const;   // 현재 콤보 인덱스
	float ResolveChargeAlpha(const UWorld* World) const;  // 차징 비율(0~1)

	// ----- 성공 커밋 -----
	void CommitActivate(const UWorld* World);

	void ResetCombo();
	void ResetCharge();

protected:
	UPROPERTY(Transient)
	TObjectPtr<USkillData> Data = nullptr;

	UPROPERTY(Transient)
	int32 Level = 1;

	// 쿨다운 종료 시각(TimeSeconds 기준)
	UPROPERTY(Transient)
	float CooldownEndTime = 0.f;

	// 콤보 상태
	UPROPERTY(Transient)
	int32 ComboIndex = 0;

	UPROPERTY(Transient)
	float ComboExpireTime = 0.f;

	// 차징 상태
	UPROPERTY(Transient)
	bool bCharging = false;

	UPROPERTY(Transient)
	float ChargeStartTime = 0.f;

	UPROPERTY(Transient)
	float CachedChargeAlpha = 0.f;
};
