// 튜토리얼 진행 상태를 정의하는 Enum
UENUM(BlueprintType)
enum class ETutorialStep : uint8
{
	IntroMove UMETA(DisplayName = "W 눌러 이동"),
	DungeonShot UMETA(DisplayName = "던전 컷씬"),
	MonsterZoom UMETA(DisplayName = "몬스터 줌인"),
	Attack UMETA(DisplayName = "좌클릭 공격"),
	Dodge UMETA(DisplayName = "쉬프트 회피"),
	Skills UMETA(DisplayName = "Q, E 스킬"),
	DoorInteract UMETA(DisplayName = "문 상호작용"),
	Completed UMETA(DisplayName = "튜토리얼 완료")
};