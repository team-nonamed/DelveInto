#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NewSystem/Enhances/PerkEffectBase.h"
#include "DamageModifier.generated.h"

enum class EWeaponSkillSlot : uint8;

UCLASS(DisplayName = "Effect: Damage Modifier")
class DELVEINTO_API UPerkEffect_DamageModifier : public UPerkEffectBase
{
	GENERATED_BODY()

public:
	virtual void OnApplied(UPerkHandler* Handler, int32 PerkLevel) override;
	virtual void OnRemoved(UPerkHandler* Handler) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	TArray<EWeaponSkillSlot> TargetSlots;

	// [신규] 발동에 필요한 콤보 타수 (0이면 콤보 무관하게 항상 발동)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition", meta = (ClampMin = "0"))
	int32 RequiredComboHit = 0;

	int32 CurrentLevel = 0;

private:
	UFUNCTION()
	void ModifyDamage(AActor* Attacker, AActor* Victim, EWeaponSkillSlot SkillSlot, int32 ComboCount, float& InOutDamage);
};