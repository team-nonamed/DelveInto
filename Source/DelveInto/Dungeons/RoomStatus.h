#pragma once

#include "CoreMinimal.h"
#include "RoomType.h"
#include "Props/RoomBase.h"
#include "RoomStatus.generated.h"

// 방의 좌표와 정보를 담을 구조체
USTRUCT(BlueprintType)
struct FRoomStatus
{
	GENERATED_BODY()

	/**
	 *  @deprecated
	 */
	UPROPERTY(BlueprintReadOnly)
	FIntPoint Coordinate; // 격자 좌표 (예: 0,0 / 1,0)
	
	UPROPERTY(BlueprintReadOnly)
	ERoomType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ARoomBase> RoomClass;
		
	UPROPERTY(BlueprintReadOnly)
	bool bIsPlayerVisited;
		
	// 순회를 위해 미리 만들어둔 변수
	bool bIsTraversed;
};