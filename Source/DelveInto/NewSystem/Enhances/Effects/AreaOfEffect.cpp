#include "AreaOfEffect.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/DamageEvents.h"
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"

void UPerkEffect_OnHitAoE::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	CurrentLevel = PerkLevel;
	Handler->OnHitTarget.AddUObject(this, &UPerkEffect_OnHitAoE::TriggerAoE);
}

void UPerkEffect_OnHitAoE::OnRemoved(UPerkHandler* Handler)
{
	Handler->OnHitTarget.RemoveAll(this);
}

void UPerkEffect_OnHitAoE::TriggerAoE(AActor* Attacker, AActor* Victim, float FinalDamage, EWeaponSkillSlot AttackSlot)
{
	if (!TargetSlots.IsEmpty() && !TargetSlots.Contains(AttackSlot)) return;
	if (!Attacker || !Victim) return;

	UWorld* World = Attacker->GetWorld();
	if (!World) return;

	// EffectValues = [0.3, 0.4, 0.5] (본 데미지 대비 계수)
	float AoEDamage = FinalDamage * GetValueForLevel(CurrentLevel);

	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(World, Victim->GetActorLocation(), Radius, ObjectTypes, nullptr, {Attacker, Victim}, OverlappedActors);

	int32 HitCount = 0;
	for (AActor* HitActor : OverlappedActors)
	{
		if (HitCount >= MaxTargets) break;
		if (HitActor)
		{
			FDamageEvent DamageEvent;
			HitActor->TakeDamage(AoEDamage, DamageEvent, Attacker->GetInstigatorController(), Attacker);
			HitCount++;
		}
	}
}