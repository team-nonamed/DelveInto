#include "RoomBase.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Utils/DirectionUtil.h"

#if WITH_EDITOR
#pragma region Display Creation

void ARoomBase::CreateArrowDisplayHolder()
{
    ArrowDisplayHolder = CreateDefaultSubobject<USceneComponent>(TEXT("Arrow Display Holder"));
    ArrowDisplayHolder->SetupAttachment(RootComponent);
    ArrowDisplayHolder->SetHiddenInGame(true);
    ArrowDisplayHolder->bIsEditorOnly = true;
}

void ARoomBase::CreateRegionDisplayHolder()
{
    RegionDisplayHolder = CreateDefaultSubobject<USceneComponent>(TEXT("Region Box Display Holder"));
    RegionDisplayHolder->SetupAttachment(RootComponent);
    RegionDisplayHolder->SetHiddenInGame(true);
    RegionDisplayHolder->bIsEditorOnly = true;
}

void ARoomBase::CreateConnectorDisplayHolder()
{
    ConnectorDisplayHolder = CreateDefaultSubobject<USceneComponent>(TEXT("Connector Display Holder"));
    ConnectorDisplayHolder->SetupAttachment(RootComponent);
    ConnectorDisplayHolder->SetHiddenInGame(true);
    ConnectorDisplayHolder->bIsEditorOnly = true;
}

void ARoomBase::CreateFloorRegionDisplay()
{
    FloorRegionDisplay = CreateDefaultSubobject<UBoxComponent>(TEXT("Floor Region Display"));
    FloorRegionDisplay->SetupAttachment(RegionDisplayHolder);
    FloorRegionDisplay->SetCollisionProfileName(TEXT("NoCollision"));
    FloorRegionDisplay->SetGenerateOverlapEvents(false);
    FloorRegionDisplay->SetHiddenInGame(true);
    FloorRegionDisplay->bIsEditorOnly = true;
    FloorRegionDisplay->ShapeColor = FColor::Cyan;
    FloorRegionDisplay->SetLineThickness(10.0f);
}

void ARoomBase::CreateDirectionArrowDisplay(ESotaDirection Direction, FColor Color)
{
    FString DirNameString = USotaDirectionUtil::GetDirectionName(Direction);
    FName ComponentName = FName(*FString::Printf(TEXT("%s Direction Arrow"), *DirNameString));

    UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(ComponentName);
    if (Arrow)
    {
       Arrow->SetupAttachment(ArrowDisplayHolder);
       Arrow->SetCollisionProfileName(TEXT("NoCollision"));
       Arrow->SetHiddenInGame(true);
       Arrow->bIsEditorOnly = true;
       Arrow->ArrowColor = Color;
       Arrow->ArrowSize = 10.0f;
       DirectionArrowDisplays.Add(Direction, Arrow);
    }
}

void ARoomBase::CreateDirectionArrowDisplays()
{
    CreateDirectionArrowDisplay(ESotaDirection::Forward, FColor::Cyan);
    CreateDirectionArrowDisplay(ESotaDirection::Right, FColor::Yellow);
    CreateDirectionArrowDisplay(ESotaDirection::Left, FColor::Yellow);
    CreateDirectionArrowDisplay(ESotaDirection::Backward, FColor::Yellow);
    CreateDirectionArrowDisplay(ESotaDirection::Center, FColor::Cyan);
}

void ARoomBase::CreateConnectorRegionDisplay(ESotaDirection Direction, FColor Color)
{
    FString DirNameString = USotaDirectionUtil::GetDirectionName(Direction);
    FName Name = FName(*FString::Printf(TEXT("%s Connector Region Display"), *DirNameString));
    
    UBoxComponent* Box = CreateDefaultSubobject<UBoxComponent>(Name);
    if (Box)
    {
       Box->SetupAttachment(RegionDisplayHolder);
       Box->SetCollisionProfileName(TEXT("NoCollision"));
       Box->SetGenerateOverlapEvents(false);
       Box->SetHiddenInGame(true);
       Box->bIsEditorOnly = true;
       Box->ShapeColor = Color;
       Box->SetLineThickness(10.0f);
       ConnectorRegionDisplays.Add(Direction, Box);
    }
}

void ARoomBase::CreateConnectorRegionDisplays()
{
    CreateConnectorRegionDisplay(ESotaDirection::Forward, FColor::Yellow);
    CreateConnectorRegionDisplay(ESotaDirection::Right, FColor::Yellow);
    CreateConnectorRegionDisplay(ESotaDirection::Left, FColor::Yellow);
    CreateConnectorRegionDisplay(ESotaDirection::Backward, FColor::Cyan);
}

void ARoomBase::CreateConnectorDisplay(ESotaDirection Direction)
{
    FString DirNameString = USotaDirectionUtil::GetDirectionName(Direction);
    FName MeshName = FName(*FString::Printf(TEXT("%s Connector Display"), *DirNameString));
    ConnectorDisplays.Add(Direction, CreateDefaultSubobject<UStaticMeshComponent>(MeshName));
    ConnectorDisplays[Direction]->SetupAttachment(ConnectorDisplayHolder);
    ConnectorDisplays[Direction]->bIsEditorOnly = true;
    ConnectorDisplays[Direction]->SetHiddenInGame(true);
    ConnectorDisplays[Direction]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARoomBase::CreateConnectorDisplays()
{
    ESotaDirection Dirs[] = {ESotaDirection::Forward, ESotaDirection::Backward, ESotaDirection::Left, ESotaDirection::Right};
    for (ESotaDirection Dir : Dirs) CreateConnectorDisplay(Dir);
}
#pragma endregion
#endif

// =========================================================================
// 생성자
// =========================================================================
ARoomBase::ARoomBase()
{
    PrimaryActorTick.bCanEverTick = false;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Center Anchor"));
	RootComponent->Mobility = EComponentMobility::Movable;

    TerrainMeshHolder = CreateDefaultSubobject<USceneComponent>(TEXT("Terrain Mesh Holder"));
    TerrainMeshHolder->SetupAttachment(RootComponent);
    
    FloorMeshComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Floor Mesh Component"));
    FloorMeshComponent->SetupAttachment(TerrainMeshHolder);
	FloorMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	FloorMeshComponent->SetCanEverAffectNavigation(true);
	
	CeilingMeshComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Ceiling Mesh Component"));
	CeilingMeshComponent->SetupAttachment(TerrainMeshHolder);
    
    WallMeshComponents.Add(ESotaDirection::Forward, CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Forward Wall Mesh Component")));
    WallMeshComponents[ESotaDirection::Forward]->SetupAttachment(TerrainMeshHolder);
    WallMeshComponents.Add(ESotaDirection::Backward, CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Backward Wall Mesh Component")));
    WallMeshComponents[ESotaDirection::Backward]->SetupAttachment(TerrainMeshHolder);
    WallMeshComponents.Add(ESotaDirection::Left, CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Left Wall Mesh Component")));
    WallMeshComponents[ESotaDirection::Left]->SetupAttachment(TerrainMeshHolder);
    WallMeshComponents.Add(ESotaDirection::Right, CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Right Wall Mesh Component")));
    WallMeshComponents[ESotaDirection::Right]->SetupAttachment(TerrainMeshHolder);

#if WITH_EDITOR
    CreateArrowDisplayHolder();
    CreateRegionDisplayHolder();
    CreateConnectorDisplayHolder();
    CreateFloorRegionDisplay();
    CreateDirectionArrowDisplays();
    CreateConnectorRegionDisplays();
    CreateConnectorDisplays();
#endif
	
	PlayerDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Player Detection Box"));
	PlayerDetectionBox->SetupAttachment(RootComponent);
	PlayerDetectionBox->SetCollisionProfileName(TEXT("Trigger"));
    
	// 기본적으로는 콜리전을 꺼둡니다. (원치 않는 초기 오버랩 방지)
	PlayerDetectionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

// =========================================================================
// 에디터 비주얼 업데이트 및 동적 세팅 (OnConstruction)
// =========================================================================
#if WITH_EDITOR
void ARoomBase::UpdateConnectorRegionDisplays(float InHalfSize)
{
    const FVector DoorHalfExtent = FVector(25.0f, 350.0f, 350.0f);
    const float DoorZ = DoorHalfExtent.Z;

    for (auto& Pair : ConnectorRegionDisplays)
    {
        ESotaDirection Dir = Pair.Key;
        UBoxComponent* Box = Pair.Value.Get();
        if (!Box) continue;

        FVector TargetLoc = FVector::ZeroVector;
        FRotator TargetRot = FRotator::ZeroRotator;

        if (EnumHasAnyFlags(Dir, ESotaDirection::Forward))  { TargetLoc = FVector(InHalfSize, 0.f, DoorZ); TargetRot = FRotator::ZeroRotator; }
        if (EnumHasAnyFlags(Dir, ESotaDirection::Backward)) { TargetLoc = FVector(-InHalfSize, 0.f, DoorZ); TargetRot = FRotator(0.f, 180.f, 0.f); }
        if (EnumHasAnyFlags(Dir, ESotaDirection::Right))    { TargetLoc = FVector(0.f, InHalfSize, DoorZ); TargetRot = FRotator(0.f, 90.f, 0.f); }
        if (EnumHasAnyFlags(Dir, ESotaDirection::Left))     { TargetLoc = FVector(0.f, -InHalfSize, DoorZ); TargetRot = FRotator(0.f, -90.f, 0.f); }

        Box->SetRelativeLocation(TargetLoc);
        Box->SetRelativeRotation(TargetRot);
        Box->SetBoxExtent(DoorHalfExtent);
    }
}

void ARoomBase::UpdateDirectionArrowDisplays(float InHalfSize)
{
    for (auto& Pair : DirectionArrowDisplays)
    {
        ESotaDirection Dir = Pair.Key;
        UArrowComponent* Arrow = Pair.Value.Get();
        if (!Arrow) continue;

        FVector TargetLoc = FVector::ZeroVector;
        FRotator TargetRot = FRotator::ZeroRotator;

        if (EnumHasAnyFlags(Dir, ESotaDirection::Forward))  { TargetLoc = FVector(InHalfSize, 0.f, 0.f); TargetRot = FRotator(0.f, 0.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Backward)) { TargetLoc = FVector(-InHalfSize, 0.f, 0.f); TargetRot = FRotator(0.f, 180.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Right))    { TargetLoc = FVector(0.f, InHalfSize, 0.f); TargetRot = FRotator(0.f, 90.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Left))     { TargetLoc = FVector(0.f, -InHalfSize, 0.f); TargetRot = FRotator(0.f, -90.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Center))   { TargetLoc = FVector::ZeroVector; TargetRot = FRotator(90.f, 0.f, 0.f); }

        Arrow->SetRelativeLocation(TargetLoc);
        Arrow->SetRelativeRotation(TargetRot);
    }
}
#endif

void ARoomBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    
    // 구조체(RoomConfig)의 Size를 참조합니다.
    float HalfSize = RoomConfig.Size * 0.5f;

    if (PlayerDetectionBox)
    {
       float BoxExtentXY = FMath::Max(50.0f, HalfSize - RoomConfig.PlayerSensorBoxMargin);
       PlayerDetectionBox->SetBoxExtent(FVector(BoxExtentXY, BoxExtentXY, 300.0f));
       PlayerDetectionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));
    }

#if WITH_EDITOR
    if (FloorRegionDisplay)
    {
       const float BoxHalfZ = 50.0f;
       FloorRegionDisplay->SetBoxExtent(FVector(HalfSize, HalfSize, BoxHalfZ));
       FloorRegionDisplay->SetRelativeLocation(FVector(0.0f, 0.0f, -BoxHalfZ));
    }

    UpdateConnectorRegionDisplays(HalfSize);
    UpdateDirectionArrowDisplays(HalfSize);

    for (auto& Pair : ConnectorDisplays)
    {
        ESotaDirection Dir = Pair.Key;
        UStaticMeshComponent* MeshComp = Pair.Value.Get();
        
        if (!MeshComp) continue;

        if (!IsConnectorPlaceable(Dir))
        {
            MeshComp->SetVisibility(false);
            continue;
        }

        MeshComp->SetVisibility(true);

        FVector TargetLoc = FVector::ZeroVector;
        FRotator TargetRot = FRotator::ZeroRotator;

        if (EnumHasAnyFlags(Dir, ESotaDirection::Forward))  { TargetLoc = FVector(HalfSize, 0.f, 0.f); TargetRot = FRotator(0.f, 0.0f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Backward)) { TargetLoc = FVector(-HalfSize, 0.f, 0.f); TargetRot = FRotator(0.f, 180.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Right))    { TargetLoc = FVector(0.f, HalfSize, 0.f); TargetRot = FRotator(0.f, 90.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Left))     { TargetLoc = FVector(0.f, -HalfSize, 0.f); TargetRot = FRotator(0.f, -90.f, 0.f); }

        MeshComp->SetRelativeLocationAndRotation(TargetLoc, TargetRot);

        TSubclassOf<ARoomConnector> SelectedClass = GetConnectorClass(Dir);
        if (SelectedClass)
        {
           if (ARoomConnector* CDO = Cast<ARoomConnector>(SelectedClass->GetDefaultObject()))
           {
              MeshComp->SetStaticMesh(CDO->GetEditorPreviewMesh());
           }
        }
        else
        {
           MeshComp->SetStaticMesh(nullptr); 
        }
    }
#endif
}

// =========================================================================
// 런타임 게임 로직 (방 진입, 전투 클리어, 스폰)
// =========================================================================
void ARoomBase::OnPlayerOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                         bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// [디버깅 로그] 어떤 녀석이 트리거를 건드렸는지 범인을 찾아냅니다!
	UE_LOG(LogTemp, Warning, TEXT("[%s] Overlapped by: %s (Component: %s)"), *GetName(), *OtherActor->GetName(), OtherComp ? *OtherComp->GetName() : TEXT("Null"));

	if (OtherActor->IsA(ADelveCharacter::StaticClass()))
	{
		// [핵심 방어 로직] 해당 액터가 현재 플레이어(사람)에 의해 조종받고 있는지 확인합니다.
		APawn* OverlappedPawn = Cast<APawn>(OtherActor);
		if (OverlappedPawn && OverlappedPawn->IsPlayerControlled())
		{
			bIsExplored = true;
			OnPlayerEnteredRoom.Broadcast(this);
			
			InitSpawners();
            
			UE_LOG(LogTemp, Log, TEXT("Player successfully entered room: %s"), *GetName());
		}
	}
}

void ARoomBase::PostInitializeComponents_Implementation()
{
	Super::PostInitializeComponents();

	// 1. 방에 부착된 모든 컴포넌트 중 IEnemySpawner 인터페이스를 가진 컴포넌트를 찾습니다.
	TArray<UActorComponent*> FoundSpawners = GetComponentsByInterface(UEnemySpawner::StaticClass());

	// 2. 찾은 컴포넌트들을 TScriptInterface 배열에 담아 캐싱합니다.
	for (UActorComponent* Comp : FoundSpawners)
	{
		// UActorComponent가 TScriptInterface 구조체로 자동 변환되어 들어갑니다.
		Spawners.Add(Comp);
	}
	
	if (RoomConfig.bUsePlayerDetectionBox && PlayerDetectionBox)
	{
		// 1. 박스 크기를 방 크기에 맞게 세팅합니다. (높이는 넉넉하게 500 등)
		float HalfSize = RoomConfig.Size * 0.5f;
		
		float BoxExtentXY = FMath::Max(50.0f, HalfSize - RoomConfig.PlayerSensorBoxMargin);
		PlayerDetectionBox->SetBoxExtent(FVector(BoxExtentXY, BoxExtentXY, 300.0f));
		PlayerDetectionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));

		// 2. 콜리전을 켭니다.
		PlayerDetectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// 3. 오버랩 이벤트를 여기서 바인딩합니다! (초기 스폰 버그 완벽 차단)
		PlayerDetectionBox->OnComponentBeginOverlap.AddDynamic(this, &ARoomBase::OnPlayerOverlap);
	}
}

bool ARoomBase::CheckRoomClear_Implementation()
{
    if (Enemies.IsEmpty())
    {
       for (auto& Pair : Connectors)
       {
          if (ARoomConnector* Connector = Pair.Value.Get())
          {
             Connector->OpenConnector(true);
          }
       }

       Cast<ADelveCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->TriggerLevelUp();
       return true;
    }

    return false;
}

void ARoomBase::InitSpawners()
{

	for (const TScriptInterface<IEnemySpawner>& Spawner: Spawners)
	{
		if (Spawner.GetObject() == nullptr)
		{
			continue;
		}
		
		ADelveEnemy* SpawnedEnemy = IEnemySpawner::Execute_SpawnEnemy(Spawner.GetObject());
		
		if (SpawnedEnemy)
		{
			SpawnedEnemy->OnEnemyDeath.AddDynamic(this, &ARoomBase::HandleEnemyDeath);
			Enemies.Add(SpawnedEnemy);
		}
	}

    if (Enemies.Num() > 0)
    {
       for (auto& Pair : Connectors)
       {
          if (ARoomConnector* Connector = Pair.Value.Get())
          {
             Connector->CloseConnector();
          }
       }
    }
}

void ARoomBase::HandleEnemyDeath_Implementation(ADelveEnemy* DeadEnemy)
{
    if (!DeadEnemy) return;
    
    UE_LOG(LogTemp, Log, TEXT("Enemy Died: %s"), *DeadEnemy->GetName());

    if (!Enemies.Remove(DeadEnemy))
    {
       UE_LOG(LogTemp, Error, TEXT("Unregistered Enemy Died: %s"), *DeadEnemy->GetName());
    }
    
    DeadEnemy->OnEnemyDeath.RemoveDynamic(this, &ARoomBase::HandleEnemyDeath);
    CheckRoomClear();
}

// =========================================================================
// HISM 바닥 및 벽 생성 로직
// =========================================================================
void ARoomBase::CreateFloorByHISM_Implementation(UStaticMesh* FloorMesh)
{
    if (!FloorMesh) return;
    
    FloorMeshComponent->ClearInstances();  
    FloorMeshComponent->SetStaticMesh(FloorMesh);

    FVector TileSize = FloorMesh->GetBoundingBox().GetSize();
    if (TileSize.X <= 0.f || TileSize.Y <= 0.f) return;

    TArray<FTransform> InstanceTransforms;
    
    // 구조체 참조
    const int32 XRepeats = FMath::FloorToInt(RoomConfig.Size / TileSize.X / 2);
    const int32 YRepeats = FMath::FloorToInt(RoomConfig.Size / TileSize.Y / 2);

    for (int i = -XRepeats; i <= XRepeats; ++i)
    {
       for (int j = -YRepeats; j <= YRepeats; ++j)
       {
          FTransform T;
          T.SetLocation(FVector(i * TileSize.X, j * TileSize.Y, 0.0));
          InstanceTransforms.Add(T);
       }
    }

    FloorMeshComponent->AddInstances(InstanceTransforms, false);
	
	// =========================================================================
	// [추가] HISM의 고질병 해결: 엔진에게 인스턴스가 추가되었으니 물리 영역을 다시 계산하라고 명령
	// =========================================================================
	// 1. 컴포넌트의 영역(Bounds)을 강제로 새로고침합니다.
	FloorMeshComponent->UpdateBounds();

	// 2. 네비게이션 시스템에 이 컴포넌트의 데이터가 변경되었음을 즉각 보고합니다.
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		NavSys->UpdateComponentInNavOctree(*FloorMeshComponent);
	}
}

void ARoomBase::CreateWallByHISM_Implementation(ESotaDirection Direction, UStaticMesh* WallMesh, int32 Height)
{
    if (Direction == ESotaDirection::Empty || Direction == ESotaDirection::Center) return;
    if (WallMesh == nullptr || Height <= 0) return;
    if (!WallMeshComponents.Contains(Direction) || WallMeshComponents[Direction] == nullptr) return;
    
    UHierarchicalInstancedStaticMeshComponent* TargetWallComponent = WallMeshComponents[Direction];

    TargetWallComponent->ClearInstances();
    TargetWallComponent->SetStaticMesh(WallMesh);

    FVector TileSize = WallMesh->GetBoundingBox().GetSize();
    if (TileSize.X <= 0.f || TileSize.Z <= 0.f) return;

    TArray<FTransform> InstanceTransforms;
    
    // 구조체 참조
    const int32 XRepeats = FMath::FloorToInt(RoomConfig.Size / TileSize.X / 2);
    float HalfSize = RoomConfig.Size / 2.0f;

    for (int i = -XRepeats; i <= XRepeats; ++i)
    {
       for (int j = 0; j < Height; ++j)
       {
          // 중앙 슬롯인지 확인
          bool bIsConnectorSlot = (i == 0 && j == 0);
          FTransform T;

          if (Direction == ESotaDirection::Forward)
          {
             // 문이 들어갈 자리면서 동시에 문이 설치가능하다면, 벽돌 배치를 제외한다.
             if (!(bIsConnectorSlot && IsConnectorPlaceable(Direction)))
             {
                T.SetLocation(FVector(HalfSize, i * TileSize.X, j * TileSize.Z));
                T.SetRotation(FQuat(FRotator(0.0f, 90.0f, 0.0f)));
                InstanceTransforms.Add(T);
             }
          }
          else if (Direction == ESotaDirection::Backward)
          {
             if (!(bIsConnectorSlot && IsConnectorPlaceable(Direction)))
             {
                T.SetLocation(FVector(-HalfSize, i * TileSize.X, j * TileSize.Z));
                T.SetRotation(FQuat(FRotator(0.0f, -90.0f, 0.0f)));
                InstanceTransforms.Add(T);
             }
          }
          else if (Direction == ESotaDirection::Left)
          {
             if (!(bIsConnectorSlot && IsConnectorPlaceable(Direction)))
             {
                T.SetLocation(FVector(i * TileSize.X, -HalfSize, j * TileSize.Z));
                T.SetRotation(FQuat(FRotator(0.0f, 0.f, 0.0f))); 
                InstanceTransforms.Add(T);
             }
          }
          else if (Direction == ESotaDirection::Right)
          {
             if (!(bIsConnectorSlot && IsConnectorPlaceable(Direction)))
             {
                T.SetLocation(FVector(i * TileSize.X, HalfSize, j * TileSize.Z));
                T.SetRotation(FQuat(FRotator(0.0f, 180.0f, 0.0f)));
                InstanceTransforms.Add(T);
             }
          }
       }
    }

    TargetWallComponent->AddInstances(InstanceTransforms, false);
}

void ARoomBase::CreateCeilingByHISM_Implementation(UStaticMesh* CeilingMesh, float CeilingHeight)
{
	if (!CeilingMesh) return;
    
	CeilingMeshComponent->ClearInstances();  
	CeilingMeshComponent->SetStaticMesh(CeilingMesh);

	FVector TileSize = CeilingMesh->GetBoundingBox().GetSize();
	if (TileSize.X <= 0.f || TileSize.Y <= 0.f) return;

	TArray<FTransform> InstanceTransforms;
    
	// 구조체 참조하여 반복 횟수 계산
	const int32 XRepeats = FMath::FloorToInt(RoomConfig.Size / TileSize.X / 2);
	const int32 YRepeats = FMath::FloorToInt(RoomConfig.Size / TileSize.Y / 2);

	for (int i = -XRepeats; i <= XRepeats; ++i)
	{
		for (int j = -YRepeats; j <= YRepeats; ++j)
		{
			FTransform T;
          
			// 1. 위치: 바닥(0.0) 대신 파라미터로 받은 높이(CeilingHeight)에 배치
			T.SetLocation(FVector(i * TileSize.X, j * TileSize.Y, CeilingHeight));
          
			// 2. 회전: 바닥용 메쉬를 천장에 그대로 쓰면 텍스처가 위쪽을 바라보므로, X축(Roll) 기준으로 180도 뒤집습니다.
			// (만약 천장 전용 양면 메쉬나, 이미 아래를 보는 메쉬라면 이 줄을 지우거나 ZeroRotator로 바꾸세요)
			T.SetRotation(FQuat(FRotator(180.0f, 0.0f, 0.0f)));
          
			InstanceTransforms.Add(T);
		}
	}

	CeilingMeshComponent->AddInstances(InstanceTransforms, false);
}

// =========================================================================
// 커넥터 및 막음벽 생성 (Spawn / Register)
// =========================================================================
ARoomConnector* ARoomBase::SpawnConnector(ESotaDirection WorldDirection, TSubclassOf<ARoomConnector> OverrideClass)
{
    FVector WorldDir = FVector::ZeroVector;
    switch (WorldDirection) {
        case ESotaDirection::Forward:  WorldDir = FVector(1, 0, 0); break;
        case ESotaDirection::Backward: WorldDir = FVector(-1, 0, 0); break;
        case ESotaDirection::Right:    WorldDir = FVector(0, 1, 0); break;
        case ESotaDirection::Left:     WorldDir = FVector(0, -1, 0); break;
        default: break;
    }
    
    FVector LocalDir = GetActorTransform().InverseTransformVectorNoScale(WorldDir);
    ESotaDirection LocalDirection = ESotaDirection::Forward;
    
    if (LocalDir.X > 0.5f) LocalDirection = ESotaDirection::Forward;
    else if (LocalDir.X < -0.5f) LocalDirection = ESotaDirection::Backward;
    else if (LocalDir.Y > 0.5f) LocalDirection = ESotaDirection::Right;
    else if (LocalDir.Y < -0.5f) LocalDirection = ESotaDirection::Left;

    TSubclassOf<ARoomConnector> ClassToSpawn = OverrideClass ? OverrideClass : GetConnectorClass(LocalDirection);
    if (!ClassToSpawn) return nullptr;

    float HalfSize = RoomConfig.Size * 0.5f;
    FVector LocalLoc = FVector::ZeroVector;
    FRotator LocalRot = FRotator::ZeroRotator;

    switch (LocalDirection) {
        case ESotaDirection::Forward:  LocalLoc = FVector(HalfSize, 0, 0); LocalRot = FRotator::ZeroRotator; break;
        case ESotaDirection::Backward: LocalLoc = FVector(-HalfSize, 0, 0); LocalRot = FRotator(0, 180.f, 0); break;
        case ESotaDirection::Right:    LocalLoc = FVector(0, HalfSize, 0); LocalRot = FRotator(0, 90.f, 0); break;
        case ESotaDirection::Left:     LocalLoc = FVector(0, -HalfSize, 0); LocalRot = FRotator(0, -90.f, 0); break;
        default: break;
    }

    FTransform LocalTransform(LocalRot, LocalLoc);
    FTransform SpawnTransform = LocalTransform * GetActorTransform();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ARoomConnector* NewConnector = GetWorld()->SpawnActor<ARoomConnector>(ClassToSpawn, SpawnTransform, SpawnParams);
    
    if (NewConnector) RegisterConnector(LocalDirection, NewConnector);
    return NewConnector;
}

void ARoomBase::RegisterConnector(ESotaDirection LocalDirection, ARoomConnector* InConnector)
{
    if (InConnector)
    {
       Connectors.Add(LocalDirection, InConnector);
    }
}

void ARoomBase::SpawnFiller(ESotaDirection WorldDirection)
{
    FVector WorldDir = FVector::ZeroVector;
    switch (WorldDirection) {
        case ESotaDirection::Forward:  WorldDir = FVector(1, 0, 0); break;
        case ESotaDirection::Backward: WorldDir = FVector(-1, 0, 0); break;
        case ESotaDirection::Right:    WorldDir = FVector(0, 1, 0); break;
        case ESotaDirection::Left:     WorldDir = FVector(0, -1, 0); break;
        default: break;
    }
    
    FVector LocalDir = GetActorTransform().InverseTransformVectorNoScale(WorldDir);
    ESotaDirection LocalDirection = ESotaDirection::Forward;
    
    if (LocalDir.X > 0.5f) LocalDirection = ESotaDirection::Forward;
    else if (LocalDir.X < -0.5f) LocalDirection = ESotaDirection::Backward;
    else if (LocalDir.Y > 0.5f) LocalDirection = ESotaDirection::Right;
    else if (LocalDir.Y < -0.5f) LocalDirection = ESotaDirection::Left;
    
    UStaticMesh* FillerMesh = GetFiller(LocalDirection);
    if (!FillerMesh) return;

    FString NameStr = FString::Printf(TEXT("FillerComponent_%d"), static_cast<int32>(LocalDirection));
    UStaticMeshComponent* NewFiller = NewObject<UStaticMeshComponent>(this, FName(*NameStr));
    
    if (NewFiller)
    {
       NewFiller->SetStaticMesh(FillerMesh);
       NewFiller->SetupAttachment(RootComponent);
        
    	float HalfSize = RoomConfig.Size * 0.5f;
    	FVector LocalLoc = FVector::ZeroVector;
    	FRotator LocalRot = FRotator::ZeroRotator;

    	// [수정] 막음용 벽(Filler)도 방향에 맞게 회전시킵니다.
    	switch (LocalDirection) {
    	case ESotaDirection::Forward:  LocalLoc = FVector(HalfSize, 0, 0); LocalRot = FRotator(0.0f, 90.f, 0.0f); break;
    	case ESotaDirection::Backward: LocalLoc = FVector(-HalfSize, 0, 0); LocalRot = FRotator(0.0f, -90.f, 0.0f); break;
    	case ESotaDirection::Right:    LocalLoc = FVector(0, HalfSize, 0); LocalRot = FRotator(0.0f, 180.f, 0.0f); break;
    	case ESotaDirection::Left:     LocalLoc = FVector(0, -HalfSize, 0); LocalRot = FRotator(0.0f, 0.f, 0.0f); break;
    	default: break;
    	}

       NewFiller->SetRelativeLocationAndRotation(LocalLoc, LocalRot);
       NewFiller->RegisterComponent();

       SpawnedFillers.Add(LocalDirection, NewFiller);
    }
}