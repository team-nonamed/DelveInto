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

	UPROPERTY(BlueprintReadOnly)
	FIntPoint Coordinate;

	UPROPERTY(BlueprintReadOnly)
	ERoomType Type;

	// 이 칸에 실제로 할당된 구체적인 클래스 (Variations 중 하나)
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ARoomBase> RoomClass;

	UPROPERTY(BlueprintReadOnly)
	ERoomVisitStatus VisitStatus = ERoomVisitStatus::Unknown;

	// 기본 생성자
	FRoomStatus() : Coordinate(0,0), Type(ERoomType::Normal), RoomClass(nullptr) {}

	// 데이터 삽입을 위한 편의 생성자
	FRoomStatus(FIntPoint InCoord, ERoomType InType, TSubclassOf<ARoomBase> InClass)
		: Coordinate(InCoord), Type(InType), RoomClass(InClass) {}
};