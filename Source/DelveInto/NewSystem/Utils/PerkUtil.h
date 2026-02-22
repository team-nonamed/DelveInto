#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "NewSystem/Enhances/Rarity.h"
#include "PerkUtil.generated.h"

UCLASS()
class DELVEINTO_API UPerkUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 희귀도 열거형(Enum)을 키값으로 변환하여 지정된 데이터 테이블에서 메타데이터를 추출합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Perk|Utility")
	static bool GetRarityData(ERarity Rarity, FRarityData& OutRarityData);
};