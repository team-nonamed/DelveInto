#pragma once


// [신규] 스킬의 현재 상태 정의 (로직 제어용)
UENUM(BlueprintType)
enum class EComboTerminationType : uint8
{
	Loop,
	LastContinue,
	Unavailable,
};