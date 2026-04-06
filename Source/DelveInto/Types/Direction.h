#pragma once

#include "CoreMinimal.h"

#include "Direction.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ESotaDirection : uint8
{
	Empty    = 0           UMETA(DisplayName = "Not Initialized"),
	Forward  = 1 << 0      UMETA(DisplayName = "Forward (North)"),			// 00001
	Backward = 1 << 1      UMETA(DisplayName = "Backward (South)"),			// 00010
	Right    = 1 << 2      UMETA(DisplayName = "Right (East)"),				// 00100
	Left     = 1 << 3      UMETA(DisplayName = "Left (West)"),				// 01000
	Center   = 1 << 4      UMETA(DisplayName = "Center (Middle)"),			// 10000
};
ENUM_CLASS_FLAGS(ESotaDirection)