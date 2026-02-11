#pragma once

UENUM(BlueprintType)
enum class ERoomDirection : uint8
{
	Forward     UMETA(DisplayName = "Forward (+X)"),
	Right       UMETA(DisplayName = "Right (+Y)"),
	Backward    UMETA(DisplayName = "Backward (-X)"),
	Left        UMETA(DisplayName = "Left (-Y)")
};