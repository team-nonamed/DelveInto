// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomStatus.h"
#include "RoomType.h"
#include "GameFramework/Actor.h"
#include "Structs/LimitCounts.h"
#include "DungeonGenerator.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRoomPreset, Display, All);

class ARoomBase;
class ADelveDoor;


USTRUCT(BlueprintType)
struct FRoomTypeConfig
{
    GENERATED_BODY()

    // 에디터에서 여러 개의 방 BP를 넣을 수 있는 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<ARoomBase>> Variations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasLimitCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bHasLimitCount", EditConditionHides, ClampMin="1", UIMin="1"))
    int32 SpawnWeight = 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bHasLimitCount", EditConditionHides))
    FLimitCounts LimitCounts;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bHasLimitCount", EditConditionHides))
    bool bIsConnectedByOneConnector = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bIsConnectedByOneConnector", EditConditionHides))
    bool bFurthestSpawn = false;
};

UCLASS()
class DELVEINTO_API ADungeonGenerator : public AActor
{
    GENERATED_BODY()

public:
    ADungeonGenerator();

protected:
    virtual void BeginPlay() override;

protected:
    int32 GetCurrentRoomCount() const { return RoomDataMap.Num(); }
    
public:
    // =================================================================
    // 설정 변수
    // =================================================================
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    FLimitCounts RoomCountLimit;

    // 방 간격 (RoomBase의 크기)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    float RoomGridSize = 6300.0f;

    // 스폰할 방 클래스 (BP_RoomBase)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    TMap<ERoomType, FRoomTypeConfig> RoomPresets;

    // 스폰할 문 클래스 (BP_DelveDoor)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    TSubclassOf<ADelveDoor> DoorClass;

private:
    // =================================================================
    // 내부 데이터 관리
    // =================================================================

    // 좌표 : 방 데이터 (논리적 데이터)
    TMap<FIntPoint, FRoomStatus> RoomDataMap;

    // 좌표 : 실제 스폰된 방 액터 (물리적 액터)
    UPROPERTY()
    TMap<FIntPoint, ARoomBase*> SpawnedRoomMap;

    // =================================================================
    // 생성 로직 함수
    // =================================================================
    
    // 1. 데이터 상에서 방 배치 (알고리즘)
    void CreateLayout();

    // [수정] bUseFurthest 옵션 추가 (기본값은 false)
    bool AttachSpecialRoom(ERoomType RoomType);
    
    // [신규] 특정 좌표 주변에 이미 배치된 방이 몇 개인지 반환
    int32 GetNeighborCount(FIntPoint Coord);


    
    // 2. 실제 방 액터 스폰
    void SpawnRooms();

    // 3. 방과 방 사이 문 스폰 및 연결
    void SpawnDoorsAndLink();

    // 헬퍼: 특정 방향의 좌표 구하기
    FIntPoint GetNeighborCoordinate(FIntPoint Current, ESotaDirection Direction);
    
    // 헬퍼: 반대 방향 구하기 (Forward <-> Backward)
    ESotaDirection GetOppositeDirection(ESotaDirection Direction);

    TSubclassOf<ARoomBase> GetRandomRoomClass(ERoomType Type);

protected:
    /** 방에서 보낸 이벤트를 처리할 함수 */
    UFUNCTION()
    void HandleRoomExplored(ARoomBase* ExploredRoom);

    /** 현재 플레이어가 위치한 방의 좌표 (미니맵 표시용) */
    UPROPERTY(BlueprintReadOnly, Category = "Dungeon State")
    FIntPoint CurrentPlayerCoordinate;
};