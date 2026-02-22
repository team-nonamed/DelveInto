#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NewSystem/Enhances/PerkEffectBase.h"

#include "Lifesteal.generated.h"


UCLASS(DisplayName = "Effect: Lifesteal")
class DELVEINTO_API UPerkEffect_Lifesteal : public UPerkEffectBase
{
	GENERATED_BODY()

public:
	virtual void OnApplied(UPerkHandler* Handler, int32 PerkLevel) override;
	virtual void OnRemoved(UPerkHandler* Handler) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lifesteal")
	float LifestealRatio = 1.0f; // 입힌 데미지의 몇 %를 흡수할 것인가

	int32 CurrentLevel = 0;

private:
	UFUNCTION()
	void ProcessLifesteal(AActor* Attacker, AActor* Victim, float FinalDamage, EWeaponSkillSlot AttackSlot);
};