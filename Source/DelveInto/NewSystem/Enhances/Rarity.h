#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Rarity.generated.h"

// 1. 희귀도 열거형 (메모리 1바이트)
UENUM(BlueprintType)
enum class ERarity : uint8
{
	Common      UMETA(DisplayName = "Common (일반)"),
	Uncommon    UMETA(DisplayName = "Uncommon (드문)"),
	Rare        UMETA(DisplayName = "Rare (희귀)"),
	Epic        UMETA(DisplayName = "Epic (영웅)"),
	Legendary   UMETA(DisplayName = "Legendary (전설)")
};

// 2. 희귀도 UI 메타데이터 (DataTable에서 사용)
USTRUCT(BlueprintType)
struct FRarityData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText RarityName; // 예: "전설"

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText RarityDescription; // 예: "매우 희귀한 기운이 느껴집니다."

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor RarityColor; // UI 텍스트나 테두리에 적용할 색상
};