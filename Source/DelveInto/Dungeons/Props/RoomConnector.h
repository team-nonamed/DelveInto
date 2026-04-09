#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomConnector.generated.h"

UCLASS(Abstract, ClassGroup=(DungeonProp))
class DELVEINTO_API ARoomConnector : public AActor
{
	GENERATED_BODY()

public:
	ARoomConnector();

	// ==============================================================
	// [핵심] 룸에서 커넥터를 제어하기 위한 공통 인터페이스
	// ==============================================================
    
	/** 커넥터를 엽니다 (예: 문 열기, 장애물 제거) */
	virtual void OpenConnector(bool bPlaySound = true) {}

	/** 커넥터를 닫습니다 (예: 문 닫기, 보스방 봉쇄) */
	virtual void CloseConnector() {}
};