#pragma once
#include "CoreMinimal.h"
#include "NewSystem/Enhances/PerkEffectBase.h"
#include "NewSystem/Weapons/WeaponData.h" // EWeaponSkillSlot
#include "CastSpeedModifier.generated.h"

UCLASS(DisplayName = "Effect: Speed Modifier")
class DELVEINTO_API UPerkEffect_CastSpeedModifier : public UPerkEffectBase
{
	GENERATED_BODY()

public:
	virtual void OnApplied(UPerkHandler* Handler, int32 PerkLevel) override;
	virtual void OnRemoved(UPerkHandler* Handler) override;

protected:
	// 속도를 올릴 대상 슬롯들 (비워두면 모든 스킬에 적용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	TArray<EWeaponSkillSlot> TargetSlots;

	int32 CurrentLevel = 0;

private:
	UFUNCTION()
	void ModifySpeed(EWeaponSkillSlot Slot, float& InOutSpeed, AActor* Instigator);
};