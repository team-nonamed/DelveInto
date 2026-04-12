#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomStatus.h"
#include "RoomType.h"
#include "Structs/LimitCounts.h"
#include "Types/Direction.h"
#include "DungeonGenerator.generated.h"

class ARoomBase;
class ARoomConnector;

// 미니맵 업데이트를 위한 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerMovedRoom);

USTRUCT(BlueprintType)
struct FRoomTypeConfig
{
    GENERATED_BODY()

    // 스폰할 방 클래스 후보군
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<ARoomBase>> Variations;

    // 최대/최소 개수 제한 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasLimitCount = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bHasLimitCount"))
    FLimitCounts LimitCounts;
    
    // 가장 먼 곳에 스폰할지 여부 (주로 보스 방)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bFurthestSpawn = false;
    
    // 문이 1개만 달리는 특수 방(막다른 방)인지 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bIsConnectedByOneConnector = false;

    // 일반 방 생성 시 가중치 (비율 계산용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnWeight = 1;
};

UCLASS()
class DELVEINTO_API ADungeonGenerator : public AActor
{
    GENERATED_BODY()

public:
    ADungeonGenerator();

protected:
    virtual void BeginPlay() override;

public:
    // =================================================================
    // 설정 변수
    // =================================================================
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    FLimitCounts RoomCountLimit;

    // 방 간격 (RoomBase의 Size와 동일하게 맞추세요. 예: 6300.0f)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    float RoomGridSize = 6300.0f;

    // 방 타입별 설정 및 프리셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    TMap<ERoomType, FRoomTypeConfig> RoomPresets;

    // =================================================================
    // 미니맵 연동 (데이터 노출)
    // =================================================================

    /** 플레이어가 새로운 방에 진입했을 때 미니맵을 갱신하기 위한 델리게이트 */
    UPROPERTY(BlueprintAssignable, Category = "Dungeon|Events")
    FOnPlayerMovedRoom OnPlayerMovedRoom;

    UFUNCTION(BlueprintPure, Category = "Dungeon|Data")
    const TMap<FIntPoint, FRoomStatus>& GetRoomDataMap() const { return RoomDataMap; }

    UFUNCTION(BlueprintPure, Category = "Dungeon|Data")
    FIntPoint GetCurrentPlayerCoordinate() const { return CurrentPlayerCoordinate; }

private:
    // =================================================================
    // 내부 데이터 관리
    // =================================================================

    // 논리적 맵 데이터
    UPROPERTY()
    TMap<FIntPoint, FRoomStatus> RoomDataMap;

    // 실제 스폰된 방 액터
    UPROPERTY()
    TMap<FIntPoint, ARoomBase*> SpawnedRoomMap;

    // 현재 플레이어가 위치한 좌표
    FIntPoint CurrentPlayerCoordinate;

    // =================================================================
    // 생성 로직 함수
    // =================================================================
    
    void CreateLayout();
    bool AttachSpecialRoom(ERoomType RoomType);
    void SpawnRooms();
    void SpawnDoorsAndLink();

    // =================================================================
    // 헬퍼 함수
    // =================================================================
    
    int32 GetNeighborCount(FIntPoint Coord);
    FIntPoint GetNeighborCoordinate(FIntPoint Current, ESotaDirection Direction);
    ESotaDirection GetOppositeDirection(ESotaDirection Direction);
    TSubclassOf<ARoomBase> GetRandomRoomClass(ERoomType Type);
    
    // 플레이어가 방에 들어왔을 때 호출될 핸들러
    UFUNCTION()
    void HandleRoomExplored(ARoomBase* ExploredRoom);
};