#pragma once

UENUM(BlueprintType)
enum class EResultType: uint8
{
	Success		UMETA(DisplayName="Success"),
	Impossible	UMETA(DisplayName="Impossible", ToolTip="Instigator가 상호작용을 요청할 수 없는 경우"),
	Invalid		UMETA(DisplayName="Invalid", ToolTip="Receiver가 상호작용이 불가능한 경우"),
	Cancelled	UMETA(DisplayName="Cancelled", ToolTip="Modifier가 요청을 취소한 경우" ),
	Nullified	UMETA(DisplayName="Nullified", ToolTip="적용될 값이 무효화 되어 전과 같은 상태를 유지한 경우"),
	Unknown		UMETA(DisplayName="Unknown"),
};
