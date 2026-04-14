#pragma once

#include "CoreMinimal.h"
#include "RoomConnector.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Dungeons/RoomType.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EnemySpawner.h"
#include "Types/Direction.h"
#include "RoomBase.generated.h"

// 매개변수로 자기 자신(ARoomBase*)을 넘겨서 어떤 방인지 알 수 있게 합니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEnteredRoom, ARoomBase*, EnteredRoom);

class ADelveDoor;

// =====================================================================
// 방 디자인 및 기본 설정을 위한 구조체
// =====================================================================
USTRUCT(BlueprintType)
struct FRoomDesignConfig
{
    GENERATED_BODY()

    /** 커넥터(문)를 설치할 수 있는 방향 설정 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "ESotaDirection"))
    int32 ConnectorPlaceableFlags = 0;

    /** 방의 전체 크기 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin="700.0", UIMin="700.0"))
    float Size = 6300.0f;

    /** 방의 정면(Forward) 기준 방향 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    ESotaDirection ForwardDirection = ESotaDirection::Forward;

    /** 미니맵 등에 사용될 방의 아이콘 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon = nullptr;
};

// =====================================================================
// 문과 벽(Filler) 세팅을 위한 구조체
// =====================================================================
USTRUCT(BlueprintType)
struct FRoomConnectorConfig
{
    GENERATED_BODY()

    /** 방향별 스폰할 커넥터(문) 클래스 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<ESotaDirection, TSubclassOf<ARoomConnector>> ConnectorClasses;

    /** 방향별 커넥터가 없을 때 막아둘 벽(Filler) 메쉬 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<ESotaDirection, TObjectPtr<UStaticMesh>> Fillers;
};


UCLASS(Abstract)
class DELVEINTO_API ARoomBase : public AActor
{
    GENERATED_BODY()
    
public:    
    ARoomBase();
    virtual void OnConstruction(const FTransform& Transform) override;

#pragma region Room Settings
protected:
    /** 방의 기본 디자인 설정 (크기, 아이콘, 설치 가능 방향 등) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room|Settings")
    FRoomDesignConfig RoomConfig;

    /** 방의 연결부(문, 막힌 벽) 프리셋 설정 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room|Settings")
    FRoomConnectorConfig ConnectorConfig;
#pragma endregion

#pragma region Room Components
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Components")
    TObjectPtr<USceneComponent> TerrainMeshHolder;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Components")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FloorMeshComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Components")
    TMap<ESotaDirection, TObjectPtr<UHierarchicalInstancedStaticMeshComponent>> WallMeshComponents;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Components")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> CeilingMeshComponent;

    /** 플레이어 진입을 감지하기 위한 트리거 박스 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Components")
    TObjectPtr<UBoxComponent> PlayerDetectionBox;
#pragma endregion

#pragma region Runtime Information
public:
    /** 미니맵에서 이 방이 방문되었는지 확인하기 위한 상태 변수 */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Runtime")
    bool bIsExplored = false;

protected:
    /** 런타임에 생성된 Connector들을 방향별로 관리 */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Runtime")
    TMap<ESotaDirection, TObjectPtr<ARoomConnector>> Connectors;

    /** 런타임에 생성된 Filler(막음벽)들을 관리 */
    UPROPERTY(VisibleInstanceOnly, Transient, Category = "Room|Runtime")
    TMap<ESotaDirection, TObjectPtr<UStaticMeshComponent>> SpawnedFillers;

    /** 런타임 방 내부의 Spawner들 */
    UPROPERTY(VisibleDefaultsOnly, Category = "Room|Runtime")
    TArray<TScriptInterface<IEnemySpawner>> Spawners;

    /** 런타임에 스폰되어 생존해 있는 Enemy 목록 */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Runtime")
    TSet<TObjectPtr<ADelveEnemy>> Enemies;
#pragma endregion

#pragma region Events
public:
    /** 플레이어가 이 방에 진입했을 때 발생하는 이벤트 */
    UPROPERTY(BlueprintAssignable, Category = "Room|Events")
    FOnPlayerEnteredRoom OnPlayerEnteredRoom;
#pragma endregion

#pragma region Getters & API Functions
public:
    UFUNCTION(BlueprintCallable, Category = "Room|Settings")
    bool IsConnectorPlaceable(ESotaDirection Direction) const { return (RoomConfig.ConnectorPlaceableFlags & static_cast<int32>(Direction)) != 0; }

    UFUNCTION(BlueprintPure, Category = "Room|Settings")
    float GetSize() const { return RoomConfig.Size; }

    UFUNCTION(BlueprintPure, Category = "Room|Settings")
    UTexture2D* GetIcon() const { return RoomConfig.Icon; }

    UFUNCTION(BlueprintPure, Category = "Room|Settings")
    FVector GetCenterRelativeLocation() const { return RootComponent->GetRelativeTransform().GetLocation(); }

    UFUNCTION(BlueprintPure, Category = "Room|Settings")
    FVector GetCenterWorldLocation() const { return RootComponent->GetComponentLocation(); }

    UFUNCTION(BlueprintPure, Category = "Room|Settings")
    FTransform GetCenterRelativeTransform() const { return RootComponent->GetRelativeTransform(); }

    UFUNCTION(BlueprintCallable, Category = "Room|Connectors")
    TSubclassOf<ARoomConnector> GetConnectorClass(ESotaDirection Direction) const
    {
       const TSubclassOf<ARoomConnector>* FoundClass = ConnectorConfig.ConnectorClasses.Find(Direction);
       return FoundClass ? *FoundClass : nullptr;
    }

    UFUNCTION(BlueprintPure, Category = "Room|Connectors")
    ARoomConnector* GetConnectorByDirection(ESotaDirection Direction) const
    {
       const TObjectPtr<ARoomConnector>* Found = Connectors.Find(Direction);
       return Found ? Found->Get() : nullptr;
    }

    UFUNCTION(BlueprintCallable, Category = "Room|Connectors")
    UStaticMesh* GetFiller(ESotaDirection Direction) const
    {
       const TObjectPtr<UStaticMesh>* FoundPtr = ConnectorConfig.Fillers.Find(Direction);
       return FoundPtr ? FoundPtr->Get() : nullptr;
    }

    UFUNCTION(BlueprintPure, Category = "Room|Runtime")
    TSet<ADelveEnemy*> GetEnemies() const 
    { 
       TSet<ADelveEnemy*> OutEnemies;
       for (const TObjectPtr<ADelveEnemy>& Enemy : Enemies) { if (Enemy) OutEnemies.Add(Enemy.Get()); }
       return OutEnemies; 
    }

    void RegisterEnemy(ADelveEnemy* InEnemy) { if (InEnemy) Enemies.Add(InEnemy); }

    UFUNCTION(BlueprintCallable, Category = "Room|Connectors")
    ARoomConnector* SpawnConnector(ESotaDirection WorldDirection, TSubclassOf<ARoomConnector> OverrideClass);

    UFUNCTION(BlueprintCallable, Category = "Room|Connectors")
    void RegisterConnector(ESotaDirection Direction, ARoomConnector* InConnector);

    UFUNCTION(BlueprintCallable, Category = "Room|Connectors")
    void SpawnFiller(ESotaDirection Direction);

    UFUNCTION(BlueprintCallable, Category = "Room|Runtime")
    void InitSpawners();
#pragma endregion

#pragma region Protected Overridable Methods
protected:
    UFUNCTION(BlueprintNativeEvent, Category = "Room")
    void PostInitializeComponents() override;
    
    UFUNCTION(BlueprintNativeEvent, Category = "Room")
    void OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                         bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Room|Runtime")
    void HandleEnemyDeath(ADelveEnemy* DeadEnemy);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Room|Runtime")
    bool CheckRoomClear();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Room|Components")
    void CreateFloorByHISM(UStaticMesh* FloorMesh);
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Room|Components")
    void CreateWallByHISM(ESotaDirection Direction, UStaticMesh* WallMesh, int32 Height);
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Room|Components")
    void CreateCeilingByHISM(UStaticMesh* CeilingMesh, float CeilingHeight);
#pragma endregion

#if WITH_EDITORONLY_DATA
#pragma region Editor Visualizers
protected:
    UPROPERTY(VisibleAnywhere, Category= "Room|Editor")
    TObjectPtr<USceneComponent> ConnectorDisplayHolder;
    
    UPROPERTY(VisibleAnywhere, Category="Room|Editor")
    TMap<ESotaDirection, TObjectPtr<UStaticMeshComponent>> ConnectorDisplays;

    UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
    TObjectPtr<USceneComponent> ArrowDisplayHolder;

    UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
    TMap<ESotaDirection, TObjectPtr<UArrowComponent>> DirectionArrowDisplays;

    UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
    TObjectPtr<USceneComponent> RegionDisplayHolder;

    UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
    TObjectPtr<UBoxComponent> FloorRegionDisplay;

    UPROPERTY(VisibleAnywhere, Category = "Room|Editor")
    TMap<ESotaDirection, TObjectPtr<UBoxComponent>> ConnectorRegionDisplays;

    void CreateArrowDisplayHolder();
    void CreateRegionDisplayHolder();
    void CreateConnectorDisplayHolder();
    void CreateFloorRegionDisplay();
    void CreateDirectionArrowDisplay(ESotaDirection Direction, FColor Color);
    void CreateDirectionArrowDisplays();
    void CreateConnectorRegionDisplay(ESotaDirection Direction, FColor Color);
    void CreateConnectorRegionDisplays();
    void CreateConnectorDisplay(ESotaDirection Direction);
    void CreateConnectorDisplays();

    void UpdateConnectorRegionDisplays(float InHalfSize);
    void UpdateDirectionArrowDisplays(float InHalfSize);
#pragma endregion
#endif

#pragma region Deprecated
// 사용하지 않는 변수들은 'Deprecated' 카테고리로 몰아 넣어서 디테일 패널 하단에 숨깁니다.
public:
    UE_DEPRECATED(1.1, "Dummy Variables")
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Deprecated")
    ESotaDirection DoorDirection = ESotaDirection::Forward;

    UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deprecated")
    FComponentReference WallForwardRef;

    UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deprecated")
    FComponentReference WallBackwardRef;

    UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deprecated")
    FComponentReference WallRightRef;

    UE_DEPRECATED(1.1, "다른 시스템으로 변경됨")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deprecated")
    FComponentReference WallLeftRef;
#pragma endregion
};