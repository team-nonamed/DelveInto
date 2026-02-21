#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None        UMETA(DisplayName = "없음"),
	Gold        UMETA(DisplayName = "금화"),
	MagicStone  UMETA(DisplayName = "마석"),
	HealthPotion UMETA(DisplayName = "체력 포션")
};