#include "PerkHandler.h"

#include "NewSystem/Enhances/PerkBase.h"
#include "NewSystem/Enhances/PerkEffectBase.h"


UPerkHandler::UPerkHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPerkHandler::AddPerk(UPerkBase* NewPerk, int32 Level)
{
	if (!NewPerk) return;

	// 이미 존재하는 퍽이면 레벨만 갱신 (기존 이펙트 OnRemoved 후 재적용 로직 추가 가능)
	if (ActivePerks.Contains(NewPerk))
	{
		// 간단한 갱신을 위해 지우고 다시 추가
		RemovePerk(NewPerk); 
	}

	ActivePerks.Add(NewPerk, Level);

	// 퍽 내부에 조립된 모든 이펙트 모듈을 실행
	for (UPerkEffectBase* Effect : NewPerk->PerkEffects)
	{
		if (Effect)
		{
			Effect->OnApplied(this, Level);
		}
	}
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