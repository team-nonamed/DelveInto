#pragma once

#include "CoreMinimal.h"
#include "Rarity.h"
#include "Engine/DeveloperSettings.h"
#include "NewSystem/Enhances/PerkBase.h"
#include "PerkSettings.generated.h"

class UPerkBase; // 전방 선언

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Perk System Settings"))
class DELVEINTO_API UPerkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// 기존에 만든 희귀도 메타데이터
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Rarity")
	TMap<ERarity, FRarityData> RarityDataMap;

	// [신규] 게임 내 존재하는 모든 퍽(PerkBase) 데이터 에셋을 등록하는 DB 배열
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Database")
	TArray<TSoftObjectPtr<UPerkBase>> RegisteredPerks;
};