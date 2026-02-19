#pragma once


// [신규] 스킬의 현재 상태 정의 (로직 제어용)
UENUM(BlueprintType)
enum class ESkillState : uint8
{
	Idle			UMETA(DisplayName = "Idle", ToolTip = "스킬 사용 가능 상태"),
	PreCharging		UMETA(DisplayName = "PreCharging", ToolTip = "Idle에서 Charging하기 위해 전환 애니메이션 재생 중 상태"),
	Charging		UMETA(DisplayName = "Charging", ToolTip = "스킬이 차징 중인 상태"),
	PostCharging	UMETA(DisplayName = "ChargedLoop", ToolTip = "스킬이 완전히 차징되어 대기 중인 상태"),
	PreCasting		UMETA(DisplayName = "PreCasting", ToolTip = "스킬을 시전하기 위해 애니메이션을 전환하는 상태"),
	Casting			UMETA(DisplayName = "Casting", ToolTip = "스킬 시전 애니메이션 재생 중 상태"),
	PostCasting		UMETA(DisplayName = "PostCasting", ToolTip = "스킬 시전 후 정리 애니메이션 재생 중 상태"),
	Cooldown		UMETA(DisplayName = "Cooldown", ToolTip = "스킬이 쿨타임 중인 상태"),
};