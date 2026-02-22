#include "PerkBase.h"

bool UPerkBase::GetLevelData(int32 Level, FPerkLevelData& OutData) const
{
	// 데이터가 비어있을 경우의 예외 처리
	if (LevelData.IsEmpty())
	{
		return false;
	}

	// "레벨 1"이 "인덱스 0"에 매칭되도록 보정
	int32 TargetIndex = Level - 1;

	// 요청된 레벨이 배열 범위를 초과하더라도 크래시가 발생하지 않도록 마지막 레벨 데이터로 강제 제한(Clamp)
	int32 SafeIndex = FMath::Clamp(TargetIndex, 0, LevelData.Num() - 1);
    
	OutData = LevelData[SafeIndex];
	return true;
}
