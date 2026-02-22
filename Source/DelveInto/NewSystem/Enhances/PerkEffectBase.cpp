#include "PerkEffectBase.h"

void UPerkEffectBase::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	// 자식 클래스에서 오버라이드하여 Delegate 바인딩 수행
}

void UPerkEffectBase::OnRemoved(UPerkHandler* Handler)
{
	// 자식 클래스에서 오버라이드하여 Delegate 바인딩 해제 수행
}

float UPerkEffectBase::GetValueForLevel(int32 Level) const
{
	// 배열 인덱스는 0부터 시작하므로 Level - 1 처리 (기획에 따라 조정 가능)
	int32 Index = Level - 1;
	if (EffectValues.IsValidIndex(Index))
	{
		return EffectValues[Index];
	}
    
	// 인덱스를 초과한 경우 가장 마지막 최대 수치 반환
	if (EffectValues.Num() > 0)
	{
		return EffectValues.Last();
	}
    
	return 0.0f;
}