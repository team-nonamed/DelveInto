#include "StatModifier.h"

void UPerkEffect_StatModifier::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	if (!Handler) return;

	// [핵심 안전장치] 이미 적용된 레벨이 있다면, 기존 수치를 먼저 빼서 초기화해 줍니다!
	if (CurrentLevel > 0)
	{
		float OldStat = GetValueForLevel(CurrentLevel);
		Handler->OnStatChanged.Broadcast(TargetStat, -OldStat);
	}

	CurrentLevel = PerkLevel;
	float StatIncrease = GetValueForLevel(CurrentLevel);

	// 디버그 로그 출력
	UE_LOG(LogTemp, Warning, TEXT("--- [PerkEffect] 스탯 변경 발생! ---"));
	UE_LOG(LogTemp, Warning, TEXT("스탯 타겟: %d | 변동 수치: %f"), (int32)TargetStat, StatIncrease);

	// 캐릭터에게 이벤트 송출
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