#include "StatModifier.h"

void UPerkEffect_StatModifier::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	if (!Handler) return;

	CurrentLevel = PerkLevel;

	// 적용할 수치 추출 (예: 체력 10 증가)
	float StatIncrease = GetValueForLevel(CurrentLevel);

	// [핵심] 캐릭터 캐스팅 없이, 이벤트만 발생시킵니다.
	Handler->OnStatChanged.Broadcast(TargetStat, StatIncrease);
}

void UPerkEffect_StatModifier::OnRemoved(UPerkHandler* Handler)
{
	if (!Handler) return;

	// 롤백할 수치 추출 (예: 올랐던 체력 10 차감)
	float StatDecrease = GetValueForLevel(CurrentLevel);

	// 음수로 만들어서 롤백 이벤트를 발생시킵니다.
	Handler->OnStatChanged.Broadcast(TargetStat, -StatDecrease);
}