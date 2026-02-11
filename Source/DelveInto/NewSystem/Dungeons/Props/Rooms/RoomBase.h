#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NewSystem/Dungeons/Props/Spawners/SimpleEnemySpawner.h" // [중요] 인터페이스 헤더 경로를 맞춰주세요
#include "NewSystem/Dungeons/RoomType.h"
#include "NewSystem/Dungeons/RoomDirection.h"
#include "RoomBase.generated.h"


class ADelveDoor;
enum class ERoomDirection : uint8;

UCLASS(Abstract)
class DELVEINTO_API ARoomBase : public AActor
{
	GENERATED_BODY()
    
public:    
	ARoomBase();
	virtual void OnConstruction(const FTransform& Transform) override;

#pragma region Properties
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Room")
	TMap<ERoomDirection, ADelveDoor*> Doors;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Room | Config")
	ERoomType Type = ERoomType::Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Room | Config")
	ERoomDirection DoorDirection = ERoomDirection::Forward;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Room | Config")
	float Size = 6300.0f;

	// 방의 중심이 될 루트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
	class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Room")
	TSet<class ADelveEnemy*> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallForwardRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallBackwardRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallRightRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallLeftRef;

private:
	UPROPERTY()
	UStaticMeshComponent* WallForward = nullptr;

	UPROPERTY()
	UStaticMeshComponent* WallBackward = nullptr;

	UPROPERTY()
	UStaticMeshComponent* WallRight = nullptr;

	UPROPERTY()
	UStaticMeshComponent* WallLeft = nullptr;
#pragma endregion

#pragma region Debug Properties

#if WITH_EDITORONLY_DATA
protected:
	// Arrow들을 묶어줄 부모 컴포넌트
	UPROPERTY()
	USceneComponent* ArrowHolder;

	// Box들을 묶어줄 부모 컴포넌트
	UPROPERTY()
	USceneComponent* BoxHolder;
	
	UPROPERTY()
	UBoxComponent* DebugFloorRegion;
	
	// [신규] 문 위치 표시용 박스 (상하좌우)
	UPROPERTY()
	UBoxComponent* DebugDoorForward;
	UPROPERTY()
	UBoxComponent* DebugDoorBackward;
	UPROPERTY()
	UBoxComponent* DebugDoorRight;
	UPROPERTY()
	UBoxComponent* DebugDoorLeft;

	// 4모서리 (대각선)와 중심 화살표
	UPROPERTY()
	UArrowComponent* ArrowFrontRight;
	UPROPERTY()
	UArrowComponent* ArrowFrontLeft;
	UPROPERTY()
	UArrowComponent* ArrowBackRight;
	UPROPERTY()
	UArrowComponent* ArrowBackLeft;
	UPROPERTY()
	UArrowComponent* ArrowCenter;

#endif
	
#pragma endregion
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Room")
	void HandleEnemyDeath(ADelveEnemy* DeadEnemy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Room")
	bool CheckRoomClear();
public:
	// [핵심 2] 스폰 트리거 메서드
	// 외부(Trigger Box 등)에서 호출할 수 있게 BlueprintCallable로 지정
	UFUNCTION(BlueprintCallable, Category = "Room Logic")
	void TrigSpawn();

	void OpenWall(ERoomDirection Direction);
};