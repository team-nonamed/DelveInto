#include "DamageModifier.h"

#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"

void UPerkEffect_DamageModifier::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	CurrentLevel = PerkLevel;
	Handler->OnCalculateDamage.AddUObject(this, &UPerkEffect_DamageModifier::ModifyDamage);
}

void UPerkEffect_DamageModifier::OnRemoved(UPerkHandler* Handler)
{
	Handler->OnCalculateDamage.RemoveAll(this);
}

// 매개변수에 ComboCount 추가
void UPerkEffect_DamageModifier::ModifyDamage(AActor* Attacker, AActor* Victim, EWeaponSkillSlot SkillSlot, int32 ComboCount, float& InOutDamage)
{
	// 1. 슬롯 검사
	if (!TargetSlots.IsEmpty() && !TargetSlots.Contains(SkillSlot)) return;

	// 2. [신규] 콤보 타수 검사 (RequiredComboHit가 0보다 클 때만 검사)
	if (RequiredComboHit > 0 && ComboCount != RequiredComboHit) return;

	// 3. 데미지 증폭 연산
	InOutDamage *= (1.0f + GetValueForLevel(CurrentLevel));
}