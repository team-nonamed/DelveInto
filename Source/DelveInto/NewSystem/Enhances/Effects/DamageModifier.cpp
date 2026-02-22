#include "DamageModifier.h"

#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"

void UPerkEffect_DamageModifier::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	CurrentLevel = PerkLevel;
	Handler->OnCalculateDamage.RemoveAll(this); 
    
	Handler->OnCalculateDamage.AddUObject(this, &UPerkEffect_DamageModifier::ModifyDamage);
}

void UPerkEffect_DamageModifier::OnRemoved(UPerkHandler* Handler)
{
	Handler->OnCalculateDamage.RemoveAll(this);
}

// 매개변수에 ComboCount 추가
void UPerkEffect_DamageModifier::ModifyDamage(AActor* Attacker, AActor* Victim, EWeaponSkillSlot SkillSlot, int32 ComboCount, float& InOutDamage)
{
	UE_LOG(LogTemp, Warning, TEXT("--- [PerkEffect] 데미지 연산 진입 ---"));
	UE_LOG(LogTemp, Warning, TEXT("원본 데미지: %f | 콤보: %d"), InOutDamage, ComboCount);

	// 1. 슬롯 검사
	if (!TargetSlots.IsEmpty() && !TargetSlots.Contains(SkillSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("-> 적용 실패: 타겟 슬롯 불일치"));
		return;
	}

	// 2. 콤보 검사
	if (RequiredComboHit > 0 && ComboCount != RequiredComboHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("-> 적용 실패: 콤보 불일치 (요구 콤보: %d)"), RequiredComboHit);
		return;
	}

	// 3. 증폭치 가져오기
	float BonusValue = GetValueForLevel(CurrentLevel);
	UE_LOG(LogTemp, Warning, TEXT("-> 조건 통과! 적용될 배율: %f"), BonusValue);

	// 4. 연산
	InOutDamage *= (1.0f + BonusValue);
	UE_LOG(LogTemp, Warning, TEXT("-> 퍽 연산 완료! 최종 데미지: %f"), InOutDamage);
}