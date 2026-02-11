#pragma once

#include "CoreMinimal.h"
#include "RoomType.h"
#include "RoomData.generated.h"

// 방의 좌표와 정보를 담을 구조체
USTRUCT(BlueprintType)
struct FRoomData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FIntPoint Coordinate; // 격자 좌표 (예: 0,0 / 1,0)

	UPROPERTY(BlueprintReadOnly)
	ERoomType Type;

	FRoomData() : Coordinate(FIntPoint(0,0)), Type(ERoomType::Normal) {}
	FRoomData(FIntPoint InCoord, ERoomType InType) : Coordinate(InCoord), Type(InType) {}
};