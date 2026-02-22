#include "CastSpeedModifier.h"
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"

void UPerkEffect_CastSpeedModifier::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	CurrentLevel = PerkLevel;
	Handler->OnModifyCastSpeed.AddUObject(this, &UPerkEffect_CastSpeedModifier::ModifySpeed);
}

void UPerkEffect_CastSpeedModifier::OnRemoved(UPerkHandler* Handler)
{
	Handler->OnModifyCastSpeed.RemoveAll(this);
}

void UPerkEffect_CastSpeedModifier::ModifySpeed(EWeaponSkillSlot Slot, float& InOutSpeed, AActor* Instigator)
{
	// 슬롯이 지정되어 있는데 현재 시전하는 슬롯이 배열에 없다면 무시
	if (!TargetSlots.IsEmpty() && !TargetSlots.Contains(Slot)) return;

	// 공격 속도 % 증가 연산 (0.1이면 10% 증가)
	InOutSpeed *= (1.0f + GetValueForLevel(CurrentLevel));
}