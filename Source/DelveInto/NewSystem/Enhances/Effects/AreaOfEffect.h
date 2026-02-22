#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NewSystem/Enhances/PerkEffectBase.h"
#include "NewSystem/Weapons/WeaponSkillSlot.h"
#include "AreaOfEffect.generated.h"

UCLASS(DisplayName = "Effect: On-Hit AoE (Rupture)")
class DELVEINTO_API UPerkEffect_OnHitAoE : public UPerkEffectBase
{
	GENERATED_BODY()

public:
	virtual void OnApplied(UPerkHandler* Handler, int32 PerkLevel) override;
	virtual void OnRemoved(UPerkHandler* Handler) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AoE")
	TArray<EWeaponSkillSlot> TargetSlots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AoE")
	float Radius = 300.0f; // 폭발 반경

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AoE")
	int32 MaxTargets = 3; // 최대 타격 대상 수

	int32 CurrentLevel = 0;

private:
	UFUNCTION()
	void TriggerAoE(AActor* Attacker, AActor* Victim, float FinalDamage, EWeaponSkillSlot AttackSlot);
};