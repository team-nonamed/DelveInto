// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NewSystem/Dungeons/RoomData.h"      // FRoomData 구조체 포함
#include "NewSystem/Dungeons/RoomDirection.h" // ERoomDirection 열거형 포함
#include "DungeonGenerator.generated.h"

class ARoomBase;
class ADelveDoor;

USTRUCT(BlueprintType)
struct FRoomClassList
{
    GENERATED_BODY()

    // 에디터에서 여러 개의 방 BP를 넣을 수 있는 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<ARoomBase>> Variations;
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
    int32 MaxRoomCount = 15;

    // 방 간격 (RoomBase의 크기)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    float RoomGridSize = 6300.0f;

    // 스폰할 방 클래스 (BP_RoomBase)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    TMap<ERoomType, FRoomClassList> RoomPresets;

    // 스폰할 문 클래스 (BP_DelveDoor)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Config")
    TSubclassOf<ADelveDoor> DoorClass;

private:
    // =================================================================
    // 내부 데이터 관리
    // =================================================================

    // 좌표 : 방 데이터 (논리적 데이터)
    TMap<FIntPoint, FRoomData> RoomDataMap;

    // 좌표 : 실제 스폰된 방 액터 (물리적 액터)
    UPROPERTY()
    TMap<FIntPoint, ARoomBase*> SpawnedRoomMap;

    // =================================================================
    // 생성 로직 함수
    // =================================================================
    
    // 1. 데이터 상에서 방 배치 (알고리즘)
    void CreateLayout();

    // 2. 실제 방 액터 스폰
    void SpawnRooms();

    // 3. 방과 방 사이 문 스폰 및 연결
    void SpawnDoorsAndLink();

    // 헬퍼: 특정 방향의 좌표 구하기
    FIntPoint GetNeighborCoordinate(FIntPoint Current, ERoomDirection Direction);
    
    // 헬퍼: 반대 방향 구하기 (Forward <-> Backward)
    ERoomDirection GetOppositeDirection(ERoomDirection Direction);

    TSubclassOf<ARoomBase> GetRandomRoomClass(ERoomType Type);
};