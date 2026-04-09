#pragma once

#include "CoreMinimal.h"
#include "ConnectorSpawner.h"
#include "RoomConnector.h"
#include "Dungeons/RoomType.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EnemySpawner.h"
#include "Types/Direction.h"
#include "RoomBase.generated.h"


// 매개변수로 자기 자신(ARoomBase*)을 넘겨서 어떤 방인지 알 수 있게 합니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEnteredRoom, ARoomBase*, EnteredRoom);

class ADelveDoor;

UCLASS(Abstract)
class DELVEINTO_API ARoomBase : public AActor
{
	GENERATED_BODY()
    
public:    
	ARoomBase();
	virtual void OnConstruction(const FTransform& Transform) override;

#pragma region Properties

#pragma region Room Info
	
#pragma region Connector Placeable Flags
	
protected:
	/**
	 * TODO: 주석 작성
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = "ESotaDirection"), Category = "Room Info")
	int32 ConnectorPlaceableFlags;

public:
	/**
	 * TODO: 주석 작성
	 * @return <code>ConnectorPlaceableFlags</code>
	 */
	UFUNCTION(BlueprintPure, Category = "Room Info")
	int32 GetConnectorPlaceableFlags() const { return ConnectorPlaceableFlags; }

	/**
	 * TODO: 주석 작성
	 * @param Direction 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "Room Info")
	bool IsConnectorPlaceable(ESotaDirection Direction) const
	{
		return (ConnectorPlaceableFlags & static_cast<int32>(Direction)) != 0;
	}
#pragma endregion

#pragma region Room Size

protected:
	/**
	 *	<p>
	 *		<code>Room</code>의 크기
	 *	</p>
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room Info")
	float Size = 6300.0f;

public:
	/**
	 *	<p>
	 *		<code>Size</code>를 반환
	 *	</p>
	 * @return <code>ARoomBase#Size</code>
	 */
	UFUNCTION(BlueprintPure, Category = "Room Info")
	float GetSize() const { return Size; }

#pragma endregion

#pragma region Room Icon
protected:
	/**
	 *	<code>Room</code>의 <code>MiniMap</code> 내 <code>Icon</code>
	 *	OPTIMIZE: 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room Info")
	TObjectPtr<UTexture2D> Icon;
	
public:
	/**
	 * TODO:
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "Room Info")
	UTexture2D* GetIcon() const { return Icon; }
#pragma endregion

#pragma region Room Center Anchor

public:
	UFUNCTION(BlueprintPure, Category = "Room Info")
	FVector GetCenterRelativeLocation() const
	{
		return RootComponent->GetRelativeTransform().GetLocation();
	}

	UFUNCTION(BlueprintPure, Category = "Room Info")
	FVector GetCenterWorldLocation() const
	{
		return RootComponent->GetComponentLocation();
	}

	UFUNCTION(BlueprintPure, Category = "Room Info")
	FTransform GetCenterRelativeTransform() const
	{
		return RootComponent->GetRelativeTransform();
	}
#pragma endregion

#pragma region Room Forward Direction

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room Info")
	ESotaDirection ForwardDirection;

#pragma endregion

#pragma endregion

#pragma region Connector Info

#pragma region Connector Spawners

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Connector Info")
	TMap<ESotaDirection, TObjectPtr<UConnectorSpawner>> ConnectorSpawners;

public:
	/**
	 * 해당 방향의 ConnectorSpawner 정보를 바탕으로 Connector를 월드에 스폰합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Connector Info")
	ARoomConnector* SpawnConnector(ESotaDirection Direction);

	/**
	 * 이미 월드에 스폰된 Connector를 이 방에 연결하고 벽을 엽니다. (이웃 방이 스폰한 경우 사용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Connector Info")
	void RegisterConnector(ESotaDirection Direction, ARoomConnector* InConnector);

public:
	UFUNCTION(BlueprintCallable, Category = "Connector Info")
	TSubclassOf<ARoomConnector> GetConnectorClass(ESotaDirection Direction) const
	{
		// 1. TMap에서 해당 방향의 Value(포인터의 포인터)를 찾습니다.
		const TObjectPtr<UConnectorSpawner>* SpawnerPtr = ConnectorSpawners.Find(Direction);

		// 2. 키가 없거나, 할당된 Spawner 오브젝트가 유효하지 않은지 검사합니다.
		if (SpawnerPtr && *SpawnerPtr)
		{
			// 3. Spawner 내부의 Getter를 호출하여 클래스를 반환합니다.
			return (*SpawnerPtr)->GetConnectorClass();
		}

		// 찾지 못했거나 유효하지 않으면 null(기본값)을 반환합니다.
		return nullptr;
	}
	
#pragma endregion

#pragma region Fillers

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Connector Info")
	TMap<ESotaDirection, TObjectPtr<UStaticMesh>> Fillers;

	/** 런타임에 생성된 Filler 컴포넌트들을 관리합니다. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Connector Info")
	TMap<ESotaDirection, TObjectPtr<UStaticMeshComponent>> SpawnedFillers;

public:
	UFUNCTION(BlueprintCallable, Category = "Connector Info")
	UStaticMesh* GetFiller(ESotaDirection Direction) const
	{
		// 1. 해당 방향의 포인터를 안전하게 찾기
		const TObjectPtr<UStaticMesh>* FoundPtr = Fillers.Find(Direction);
        
		if (!FoundPtr)
		{
			return nullptr; // 해당 방향에 정의된 Filler가 없음
		}

		return FoundPtr->Get();
	}

	/**
	 * 해당 방향에 Connector 대신 Filler(막음용 벽) 메쉬를 생성합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Connector Info")
	void SpawnFiller(ESotaDirection Direction);
	
#pragma endregion

#pragma region Connectors

protected:
	/** * 런타임에 생성된 Connector들을 방향별로 관리
	 * Key: 생성 위치 방향, Value: 생성된 액터 포인터
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Connector Info")
	TMap<ESotaDirection, TObjectPtr<ARoomConnector>> Connectors;

public:
	/** 특정 방향의 커넥터를 안전하게 가져옵니다. */
	UFUNCTION(BlueprintPure, Category = "Connector Info")
	ARoomConnector* GetConnectorByDirection(ESotaDirection Direction) const
	{
		const TObjectPtr<ARoomConnector>* Found = Connectors.Find(Direction);
		return Found ? Found->Get() : nullptr;
	}
	
#pragma endregion

#pragma endregion

#pragma region Spawner Info

#pragma region Spawners

protected:
	/** * <p><code>Room</code> 내부에 배치된 <code>Spawner</code>들입니다.</p>
	 * 외부 노출 없이 내부 로직용으로만 관리됩니다.
	 */
	UPROPERTY(VisibleInstanceOnly, Category = "Spawner Info")
	TSet<TObjectPtr<UEnemySpawner>> Spawners;

#pragma endregion

#pragma region Enemies
	/**
	 *	<p>런타임에 <code>Room</code> 내부에 존재하는 <code>Enemy</code> 목록입니다.</p>
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spawner Info")
	TSet<TObjectPtr<ADelveEnemy>> Enemies;

public:
	/** 현재 방에 존재하는 모든 적(Enemy) 목록을 반환합니다. */
	UFUNCTION(BlueprintPure, Category = "Room|Spawner")
	TSet<ADelveEnemy*> GetEnemies() const 
	{ 
		// TSet<TObjectPtr<T>>는 TSet<T*>로 안전하게 복사/변환됩니다.
		TSet<ADelveEnemy*> OutEnemies;
		for (const TObjectPtr<ADelveEnemy>& Enemy : Enemies)
		{
			if (Enemy) OutEnemies.Add(Enemy.Get());
		}
		return OutEnemies; 
	}

	/** 외부에서 적이 스폰되었을 때 목록에 등록하기 위한 함수입니다. */
	void RegisterEnemy(ADelveEnemy* InEnemy) { if (InEnemy) Enemies.Add(InEnemy); }
#pragma endregion

#pragma endregion

#if WITH_EDITORONLY_DATA
#pragma region Editor Only

protected:
	/** Room 내의 모든 Arrow의 Holder 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
	TObjectPtr<USceneComponent> ArrowDisplayHolder;

	/** Room 내의 방향 표시자 (ESotaDirection별 화살표 컴포넌트) */
	UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
	TMap<ESotaDirection, TObjectPtr<UArrowComponent>> DirectionArrowDisplays;

	/** Room 내의 모든 영역 표시 Box의 Holder 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
	TObjectPtr<USceneComponent> RegionDisplayHolder;

	/** Room 내의 바닥 영역 표시용 디버그 박스 */
	UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
	TObjectPtr<UBoxComponent> FloorRegionDisplay;

	/** Room 내의 Connector 위치 표시용 디버그 박스들 */
	UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
	TMap<ESotaDirection, TObjectPtr<UBoxComponent>> ConnectorRegionDisplays;
	
#pragma endregion
#endif

#pragma region Room Triggers

protected:
	/** 플레이어 진입을 감지하기 위한 트리거 박스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room Info")
	TObjectPtr<UBoxComponent> PlayerDetectionBox;

	/** 플레이어가 이 방에 들어왔을 때 호출될 핸들러 */
	UFUNCTION()
	virtual void OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								bool bFromSweep, const FHitResult& SweepResult);

public:
	/** 미니맵에서 이 방이 방문되었는지 확인하기 위한 상태 변수 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room Info")
	bool bIsExplored = false;

	/** 플레이어가 이 방에 진입했을 때 발생하는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Room Events")
	FOnPlayerEnteredRoom OnPlayerEnteredRoom;
	
#pragma endregion

	
#pragma region deprecated
public:
	/**
	 *	TODO: Deprecated
	 *	@deprecated 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Room")
	TMap<ESotaDirection, ADelveDoor*> Doors;

protected:
	/**
	 *	TODO: Deprecated
	 *	@deprecated 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Room | Config")
	ERoomType Type = ERoomType::Normal;

	/**
	 *	TODO: Deprecated
	 *	@deprecated 
	 */
	UE_DEPRECATED(1.1, "Dummy Variables")
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Room | Config")
	ESotaDirection DoorDirection = ESotaDirection::Forward;

	/**
	 *	@deprecated 
	 */
	UE_DEPRECATED(5.6, "RootScene은 폐기되었습니다. CenterAnchor를 사용하세요.")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room", meta=(DeprecatedProperty, DeprecationMessage="Use CenterAnchor instead"))
	class USceneComponent* RootScene;

	/**
	 *	TODO: Deprecated
	 *	@deprecated 
	 */
	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallForwardRef;

	/**
	 *	TODO: Deprecated
	 *	@deprecated 
	 */
	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallBackwardRef;

	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallRightRef;

	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Visuals")
	FComponentReference WallLeftRef;

private:
	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY()
	UStaticMeshComponent* WallForward = nullptr;

	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY()
	UStaticMeshComponent* WallBackward = nullptr;

	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY()
	UStaticMeshComponent* WallRight = nullptr;

	UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
	UPROPERTY()
	UStaticMeshComponent* WallLeft = nullptr;

#pragma region Debug Properties

#if WITH_EDITORONLY_DATA
protected:
	// Arrow들을 묶어줄 부모 컴포넌트
	UE_DEPRECATED(1.1, "Use ArrowDisplayHolder")
	UPROPERTY()
	USceneComponent* ArrowHolder;

	// Box들을 묶어줄 부모 컴포넌트
	UE_DEPRECATED(1.1, "Migrate to RegionDisplayHolder")
	UPROPERTY()
	USceneComponent* BoxHolder;

	UE_DEPRECATED(1.1, "Migrate to FloorRegionDisplay")
	UPROPERTY()
	UBoxComponent* DebugFloorRegion;
	
	// [신규] 문 위치 표시용 박스 (상하좌우)
	UE_DEPRECATED(1.1, "Migrate to ConnectorRegionDisplays")
	UPROPERTY()
	UBoxComponent* DebugDoorForward;
	UE_DEPRECATED(1.1, "Migrate to ConnectorRegionDisplays")
	UPROPERTY()
	UBoxComponent* DebugDoorBackward;
	UE_DEPRECATED(1.1, "Migrate to ConnectorRegionDisplays")
	UPROPERTY()
	UBoxComponent* DebugDoorRight;
	UE_DEPRECATED(1.1, "Migrate to ConnectorRegionDisplays")
	UPROPERTY()
	UBoxComponent* DebugDoorLeft;

	// 4모서리 (대각선)와 중심 화살표
	UE_DEPRECATED(1.1, "Migrate to DirectionArrowDisplays")
	UPROPERTY()
	UArrowComponent* ArrowFrontRight;
	UE_DEPRECATED(1.1, "Migrate to DirectionArrowDisplays")
	UPROPERTY()
	UArrowComponent* ArrowFrontLeft;
	UE_DEPRECATED(1.1, "Migrate to DirectionArrowDisplays")
	UPROPERTY()
	UArrowComponent* ArrowBackRight;
	UE_DEPRECATED(1.1, "Migrate to DirectionArrowDisplays")
	UPROPERTY()
	UArrowComponent* ArrowBackLeft;
	UE_DEPRECATED(1.1, "Migrate to DirectionArrowDisplays")
	UPROPERTY()
	UArrowComponent* ArrowCenter;

#endif
	
#pragma endregion

#pragma endregion

#pragma endregion
	
#pragma region Inner Methods
	
#if WITH_EDITORONLY_DATA
#pragma region Editor Visualizer Creation Functions

protected:
	void CreateArrowDisplayHolder();
	void CreateRegionDisplayHolder();
	void CreateFloorRegionDisplay();

	void CreateDirectionArrowDisplay(ESotaDirection Direction, FColor Color);
	void CreateDirectionArrowDisplays();

	void CreateConnectorRegionDisplay(ESotaDirection Direction, FColor Color);
	void CreateConnectorRegionDisplays();
	
#pragma endregion

#pragma region Editor Visualizer Update Functions

protected:
	void UpdateConnectorRegionDisplays(float InHalfSize);
	void UpdateDirectionArrowDisplays(float InHalfSize);
	
#pragma endregion
#endif

#pragma region Connector Spawner Creation Methods
	
protected:
	void CreateConnectorSpawner(ESotaDirection Direction, FName Name);
	void CreateConnectorSpawners();
	
#pragma endregion

#pragma region Connector Spawner Update Methods

protected:
	void UpdateConnectorSpawners(float InHalfSize);
	
#pragma endregion
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

	void OpenWall(ESotaDirection Direction);
};