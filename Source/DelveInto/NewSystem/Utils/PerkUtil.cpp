#include "PerkUtil.h"

#include "NewSystem/Enhances/PerkSettings.h"

bool UPerkUtil::GetRarityData(ERarity Rarity, FRarityData& OutRarityData)
{
	// 프로젝트 세팅 데이터에 전역 접근 (매우 가볍고 빠름)
	const UPerkSettings* Settings = GetDefault<UPerkSettings>();
	if (!Settings) return false;

	// TMap에서 Enum을 키값으로 검색
	if (const FRarityData* FoundData = Settings->RarityDataMap.Find(Rarity))
	{
		OutRarityData = *FoundData;
		return true;
	}

	return false;
}
