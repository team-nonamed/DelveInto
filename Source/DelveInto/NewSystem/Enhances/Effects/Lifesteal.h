#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NewSystem/Enhances/PerkEffectBase.h"

#include "Lifesteal.generated.h"

// [신규] 흡혈 방식을 결정하는 열거형(Enum)
UENUM(BlueprintType)
enum class ELifestealMode : uint8
{
	Percentage UMETA(DisplayName = "Percentage (비율)"),
	FlatAmount UMETA(DisplayName = "Flat Amount (절댓값)")
};

UCLASS(DisplayName = "Effect: Lifesteal")
class DELVEINTO_API UPerkEffect_Lifesteal : public UPerkEffectBase
{
	GENERATED_BODY()

public:
	virtual void OnApplied(UPerkHandler* Handler, int32 PerkLevel) override;
	virtual void OnRemoved(UPerkHandler* Handler) override;

protected:
	// [신규] 에디터에서 흡혈 방식을 선택하는 드롭다운
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lifesteal")
	ELifestealMode LifestealMode = ELifestealMode::Percentage;

	// [수정] 모드가 'Percentage'일 때만 에디터에 보입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lifesteal", meta = (EditCondition = "LifestealMode == ELifestealMode::Percentage", EditConditionHides))
	float LifestealRatio = 1.0f; // 입힌 데미지의 몇 배를 흡수할 것인가 (예: 0.1 = 10%)

	// [신규] 모드가 'FlatAmount'일 때만 에디터에 보입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lifesteal", meta = (EditCondition = "LifestealMode == ELifestealMode::FlatAmount", EditConditionHides))
	float FlatHealAmount = 10.0f; // 타격 시 고정적으로 회복할 수치

	int32 CurrentLevel = 0;

private:
	UFUNCTION()
	void ProcessLifesteal(AActor* Attacker, AActor* Victim, float FinalDamage, EWeaponSkillSlot AttackSlot);
};