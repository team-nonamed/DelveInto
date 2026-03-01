#include "Lifesteal.h"
#include "Math/UnrealMathUtility.h"
#include "NewSystem/Entities/Characters/Handlers/HealthHandler.h"
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"

void UPerkEffect_Lifesteal::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	CurrentLevel = PerkLevel;
	Handler->OnHitTarget.AddUObject(this, &UPerkEffect_Lifesteal::ProcessLifesteal);
}

void UPerkEffect_Lifesteal::OnRemoved(UPerkHandler* Handler)
{
	Handler->OnHitTarget.RemoveAll(this);
}

void UPerkEffect_Lifesteal::ProcessLifesteal(AActor* Attacker, AActor* Victim, float FinalDamage, EWeaponSkillSlot AttackSlot)
{
	if (!Attacker || FinalDamage <= 0.0f) return;

	// EffectValues = [0.01, 0.02, 0.03, 0.05] (발동 확률)
	float Chance = GetValueForLevel(CurrentLevel);

	if (FMath::FRand() <= Chance)
	{
		float HealAmount = 0.0f;

		// [핵심] 선택된 모드에 따라 회복량 계산 방식을 분기 처리합니다.
		if (LifestealMode == ELifestealMode::Percentage)
		{
			HealAmount = FinalDamage * LifestealRatio;
		}
		else if (LifestealMode == ELifestealMode::FlatAmount)
		{
			HealAmount = FlatHealAmount;
		}

		// 회복량이 0보다 클 때만 체력 컴포넌트를 찾아 적용
		if (HealAmount > 0.0f)
		{
			UHealthHandler* HealthComp = Attacker->FindComponentByClass<UHealthHandler>();
			if (HealthComp) 
			{
				HealthComp->ApplyHeal(HealAmount);
			}
		}
	}
}