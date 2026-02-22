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
		float HealAmount = FinalDamage * LifestealRatio;
		
		// 캐릭터의 체력 컴포넌트를 탐색하여 HealAmount만큼 회복시키는 로직 호출
		UHealthHandler* HealthComp = Attacker->FindComponentByClass<UHealthHandler>();
		if (HealthComp) HealthComp->ApplyHeal(HealAmount);
	}
}