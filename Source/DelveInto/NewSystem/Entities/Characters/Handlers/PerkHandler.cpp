#include "PerkHandler.h"

#include "NewSystem/Enhances/PerkBase.h"
#include "NewSystem/Enhances/PerkEffectBase.h"

DEFINE_LOG_CATEGORY(LogPerk);

UPerkHandler::UPerkHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPerkHandler::AddPerk(UPerkBase* Perk, int32 Level)
{
	if (!Perk)
	{
		UE_LOG(LogPerk, Error, TEXT("유효하지 않은 Perk이 들어왔습니다."))
	}

	// 1. 보유 중인 퍽 목록에 레벨 갱신
	ActivePerks.Add(Perk, Level);

	// 2. [매우 중요] 이 퍽이 가진 기능 모듈(Effect)들을 실제로 발동(Apply)시킵니다!
	for (UPerkEffectBase* Effect : Perk->PerkEffects)
	{
		if (Effect)
		{
			// 이전에 만들어둔 SpeedModifier, DamageModifier 등의 OnApplied가 여기서 실행되며 
			// 캐릭터의 델리게이트에 자신을 바인딩합니다.
			Effect->OnApplied(this, Level);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("퍽 적용 완료: %s (Lv.%d)"), *Perk->PerkName.ToString(), Level);
}

void UPerkHandler::RemovePerk(UPerkBase* PerkToRemove)
{
	if (!PerkToRemove || !ActivePerks.Contains(PerkToRemove)) return;

	for (UPerkEffectBase* Effect : PerkToRemove->PerkEffects)
	{
		if (Effect)
		{
			Effect->OnRemoved(this);
		}
	}

	ActivePerks.Remove(PerkToRemove);
}

int UPerkHandler::GetPerkLevel(UPerkBase* Perk)
{
	if (Perk && ActivePerks.Contains(Perk))
	{
		return ActivePerks.FindRef(Perk);
	}

	return 0;
}
