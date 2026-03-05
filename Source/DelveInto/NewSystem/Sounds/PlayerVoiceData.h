#pragma once
#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "Engine/DataTable.h" // 데이터 테이블을 쓰기 위해 필수
#include "PlayerVoiceData.generated.h"

// 1. 대사가 나오는 '상황'을 정의하는 Enum
UENUM(BlueprintType)
enum class EVoiceSituation : uint8
{
	EnterDungeon     UMETA(DisplayName = "1. 던전 진입 (PlayerEnter)"),
	FindChest        UMETA(DisplayName = "2. 상자 발견 (PlayerChest)"),
	OpenChestSuccess UMETA(DisplayName = "3. 상자 열기 성공 (PlayerChestSuccess)"),
	TakeDamage       UMETA(DisplayName = "4. 피격 (PlayerHit)"),
	Die              UMETA(DisplayName = "5. 사망 (PlayerDead)"),
	ClearDungeon     UMETA(DisplayName = "6. 던전 클리어 (PlayerDungeonClear)"),
	MeetMerchant     UMETA(DisplayName = "7. 상인 조우 (PlayerMerchantMeet)"),
	SelectPerk       UMETA(DisplayName = "8. 퍽 선택 (PlayerPerkSelect)")
};

// 2. 데이터 테이블의 행(Row)이 될 구조체
USTRUCT(BlueprintType)
struct FPlayerVoiceData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 이 대사가 어느 상황에 쓰이는가?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	EVoiceSituation Situation;

	// 재생할 FMOD 사운드 이벤트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	UFMODEvent* VoiceEvent;

	// 화면에 띄울 자막 (FText를 써야 나중에 언어 번역 시스템을 붙일 수 있습니다)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice", meta = (MultiLine = true))
	FText SubtitleText;

	// 자막을 화면에 띄워둘 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	float SubtitleDuration = 3.0f;
};