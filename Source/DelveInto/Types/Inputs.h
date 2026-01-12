#pragma once

UENUM(BlueprintType)
enum class EInputSignalType: uint8
{
	Move UMETA(DisplayName = "Move Action"),
	Hurt UMETA(DisplayName = "Hurt")
};