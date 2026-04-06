 // Fill out your copyright notice in the Description page of Project Settings.


#include "RoomBase.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NewSystem/DelveDoor.h"
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

	// 2. 컴포넌트 생성 및 설정
	UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(ComponentName);
    
	if (Arrow)
	{
		Arrow->SetupAttachment(ArrowDisplayHolder);
		Arrow->SetCollisionProfileName(TEXT("NoCollision"));
		Arrow->SetHiddenInGame(true);
		Arrow->bIsEditorOnly = true;
		Arrow->ArrowColor = Color;
		Arrow->ArrowSize = 10.0f;

		// 3. TMap에 등록
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
	// 1. 이름 포맷팅: <Forward> Direction Arrow 형식
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

		// TMap에 즉시 등록
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

#pragma endregion
#endif

#pragma region Connector Spawner Creation Methods
void ARoomBase::CreateConnectorSpawner(ESotaDirection Direction, FName Name)
{
	UConnectorSpawner* Spawner = CreateDefaultSubobject<UConnectorSpawner>(Name);
	Spawner->SetupAttachment(RootComponent);
	ConnectorSpawners.Add(Direction, Spawner);
}

void ARoomBase::CreateConnectorSpawners()
{
	CreateConnectorSpawner(ESotaDirection::Forward, TEXT("Forward Spawner"));
	CreateConnectorSpawner(ESotaDirection::Backward, TEXT("Backward Spawner"));
	CreateConnectorSpawner(ESotaDirection::Right, TEXT("Right Spawner"));
	CreateConnectorSpawner(ESotaDirection::Left, TEXT("Left Spawner"));
}


#pragma endregion

 /**
  *	<p>
  *		RoomBase의 Root Component와 Editor 상에서 표시하기 위한 Display를 초기화
  *	</p>
  */
ARoomBase::ARoomBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root Component 설정
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Center Anchor"));

#if WITH_EDITOR
	
	/**
	 *	Room의 방향을 나타내기 위한 Arrow Component들을 담아두기 위한 Holder를 생성하고
	 *	Room의 영역을 나타내기 위한 Box Component들을 담아두기 위한 Holder를 생성하며
	 *	Room의 방향을 나타내는 Arrow Component들을 생성한 후
	 *	Room의 바닥 영역을 표시하는 Box Component를 생성하고
	 *	Room의 connector 영역을 표시하는 Box Component를 생성
	 */
#pragma region Create Editor Visualizers
	

	CreateArrowDisplayHolder();
	CreateRegionDisplayHolder();
	CreateFloorRegionDisplay();
	CreateDirectionArrowDisplays();
	CreateConnectorRegionDisplays();


#pragma endregion

#endif

	CreateConnectorSpawners();
}


#if WITH_EDITOR

// Visualize를 위한 Component를 업데이트하는 Method
#pragma region Editor Visualizer Update Methods

 /**
  * <p>
  *		<code>Room</code>에 속하는 <code>Connector</code>의 영역을 업데이트
  * </p>
  * @param InHalfSize <code>Room</code>의 절반 크기
  * @todo  문의 크기를 RoomBase의 책임으로 넘기기
  */
void ARoomBase::UpdateConnectorRegionDisplays(float InHalfSize)
{
    // 요구사항: 높이 700, 가로 700, 깊이 50 -> 반지름 값으로 계산
    const FVector DoorHalfExtent = FVector(25.0f, 350.0f, 350.0f);
    const float DoorZ = DoorHalfExtent.Z;

    for (auto& Pair : ConnectorRegionDisplays)
    {
        ESotaDirection Dir = Pair.Key;
        UBoxComponent* Box = Pair.Value.Get();
        if (!Box) continue;

        FVector TargetLoc = FVector::ZeroVector;
        FRotator TargetRot = FRotator::ZeroRotator;

        // 방향 비트에 따른 위치 및 회전값 계산
        if (EnumHasAnyFlags(Dir, ESotaDirection::Forward))  { TargetLoc = FVector(InHalfSize, 0.f, DoorZ); TargetRot = FRotator::ZeroRotator; }
        if (EnumHasAnyFlags(Dir, ESotaDirection::Backward)) { TargetLoc = FVector(-InHalfSize, 0.f, DoorZ); TargetRot = FRotator(0.f, 180.f, 0.f); }
        if (EnumHasAnyFlags(Dir, ESotaDirection::Right))    { TargetLoc = FVector(0.f, InHalfSize, DoorZ); TargetRot = FRotator(0.f, 90.f, 0.f); }
        if (EnumHasAnyFlags(Dir, ESotaDirection::Left))     { TargetLoc = FVector(0.f, -InHalfSize, DoorZ); TargetRot = FRotator(0.f, -90.f, 0.f); }

        Box->SetRelativeLocation(TargetLoc);
        Box->SetRelativeRotation(TargetRot);
        Box->SetBoxExtent(DoorHalfExtent);
    }
}

 /**
  * <p>
  *		<code>Room</code>에 속하는 <Code>Direction Arrow</code>를 업데이트
  * </p>
  * @param InHalfSize <code>Room</code>의 절반 크기
  */
void ARoomBase::UpdateDirectionArrowDisplays(float InHalfSize)
{
    for (auto& Pair : DirectionArrowDisplays)
    {
        ESotaDirection Dir = Pair.Key;
        UArrowComponent* Arrow = Pair.Value.Get();
        if (!Arrow) continue;

        FVector TargetLoc = FVector::ZeroVector;
        FRotator TargetRot = FRotator::ZeroRotator;

        // 비트마스크를 활용하여 방향별 좌표 및 회전 설정
        if (EnumHasAnyFlags(Dir, ESotaDirection::Forward))  { TargetLoc = FVector(InHalfSize, 0.f, 0.f); TargetRot = FRotator(0.f, 0.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Backward)) { TargetLoc = FVector(-InHalfSize, 0.f, 0.f); TargetRot = FRotator(0.f, 180.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Right))    { TargetLoc = FVector(0.f, InHalfSize, 0.f); TargetRot = FRotator(0.f, 90.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Left))     { TargetLoc = FVector(0.f, -InHalfSize, 0.f); TargetRot = FRotator(0.f, -90.f, 0.f); }
        else if (EnumHasAnyFlags(Dir, ESotaDirection::Center))   { TargetLoc = FVector::ZeroVector; TargetRot = FRotator(90.f, 0.f, 0.f); } // 위를 바라봄

        Arrow->SetRelativeLocation(TargetLoc);
        Arrow->SetRelativeRotation(TargetRot);
    }
}


#pragma endregion

#endif

#pragma region Connector Spawner Update Methods

void ARoomBase::UpdateConnectorSpawners(float InHalfSize)
{
	// 디버그 박스와 동일하게 방의 끝자락(문이 달릴 위치)을 계산
	const FVector DoorHalfExtent = FVector(25.0f, 350.0f, 350.0f);
	const float DoorZ = DoorHalfExtent.Z;

	for (auto& Pair : ConnectorSpawners)
	{
		ESotaDirection Dir = Pair.Key;
		UConnectorSpawner* Spawner = Pair.Value.Get();
		if (!Spawner) continue;

		// 1. 활성화/비활성화 제어 (디버그 박스와 동일한 논리 적용)
		bool bIsPlaceable = IsConnectorPlaceable(Dir);
		Spawner->SetActive(bIsPlaceable);
		// 에디터 상에서 비활성화된 스포너 컴포넌트를 숨겨 헷갈리지 않게 함
		Spawner->SetHiddenInGame(!bIsPlaceable); 

		// 2. 위치 동기화
		if (bIsPlaceable)
		{
			FVector TargetLoc = FVector::ZeroVector;
			FRotator TargetRot = FRotator::ZeroRotator;

			if (EnumHasAnyFlags(Dir, ESotaDirection::Forward))  { TargetLoc = FVector(InHalfSize, 0.f, DoorZ); TargetRot = FRotator::ZeroRotator; }
			else if (EnumHasAnyFlags(Dir, ESotaDirection::Backward)) { TargetLoc = FVector(-InHalfSize, 0.f, DoorZ); TargetRot = FRotator(0.f, 180.f, 0.f); }
			else if (EnumHasAnyFlags(Dir, ESotaDirection::Right))    { TargetLoc = FVector(0.f, InHalfSize, DoorZ); TargetRot = FRotator(0.f, 90.f, 0.f); }
			else if (EnumHasAnyFlags(Dir, ESotaDirection::Left))     { TargetLoc = FVector(0.f, -InHalfSize, DoorZ); TargetRot = FRotator(0.f, -90.f, 0.f); }

			Spawner->SetRelativeLocation(TargetLoc);
			Spawner->SetRelativeRotation(TargetRot);
		}
	}
}

#pragma endregion


 void ARoomBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

	// deprecated
	// WallForward = Cast<UStaticMeshComponent>(WallForwardRef.GetComponent(this));
	// WallBackward = Cast<UStaticMeshComponent>(WallBackwardRef.GetComponent(this));
	// WallRight = Cast<UStaticMeshComponent>(WallRightRef.GetComponent(this));
	// WallLeft = Cast<UStaticMeshComponent>(WallLeftRef.GetComponent(this));

#if WITH_EDITOR
	
    // Room의 HalfSize를 구함
	float HalfSize = Size * 0.5f;

	// 바닥 영역을 나타내는 박스의 크기와 위치 업데이트
#pragma region Update Floor Region
	if (FloorRegionDisplay)
	{
		// 반지름 개념이므로 절반값 적용, 두께는 50.0f
		const float BoxHalfZ = 50.0f;
		FloorRegionDisplay->SetBoxExtent(FVector(HalfSize, HalfSize, BoxHalfZ));
        
		// 윗면이 0(바닥 평면)이 되도록 두께 절반만큼 아래로 배치
		FloorRegionDisplay->SetRelativeLocation(FVector(0.0f, 0.0f, -BoxHalfZ));
	}
#pragma endregion

	// 각 방향별 연결부 영역 배치 갱신
#pragma region Update Connector Regions
	
	UpdateConnectorRegionDisplays(HalfSize);
#pragma endregion

	// 각 방향별 화살표 위치 및 회전 갱신
#pragma region Update Direction Arrows
	
	UpdateDirectionArrowDisplays(HalfSize);
#pragma endregion
#endif

	UpdateConnectorSpawners(HalfSize);
}

bool ARoomBase::CheckRoomClear_Implementation()
{
	if (Enemies.IsEmpty())
	{
		for (auto& Pairs : Doors)
		{
			if (Pairs.Value)
			{
				
				
				Pairs.Value->OpenDoor(true);
			}
		}

		Cast<ADelveCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->TriggerLevelUp();
		return true;
	}

	return false;
}

void ARoomBase::TrigSpawn()
{
	// TODO: Template을 이용하든 바로 UEnemySpawner로 받을 수 있는지 확인하기
	TArray<UActorComponent*> SpawnerComponents = GetComponentsByInterface(UEnemySpawner::StaticClass());

	for (UActorComponent* Comp : SpawnerComponents)
	{
		if (Comp)
		{
			// 2. 적 소환 (리턴값으로 소환된 적을 받음)
			ADelveEnemy* SpawnedEnemy = IEnemySpawner::Execute_SpawnEnemy(Comp);

			// 3. 소환에 성공했다면 델리게이트 바인딩
			if (SpawnedEnemy)
			{
				// [핵심] AddDynamic(대상객체, &클래스명::함수명)
				SpawnedEnemy->OnEnemyDeath.AddDynamic(this, &ARoomBase::HandleEnemyDeath);
                
				Enemies.Add(SpawnedEnemy);
			}
		}
	}

	if (Enemies.Num() > 0)
	{
		for (auto& Pairs: Doors)
		{
			if (Pairs.Value)
			{
				Pairs.Value->CloseDoor();
			}
		}
	}
}

// 4. 적이 죽었을 때 실행될 로직
void ARoomBase::HandleEnemyDeath_Implementation(ADelveEnemy* DeadEnemy)
{
	if (!DeadEnemy)
	{
		return;
	}
	
	// 로그 확인
	UE_LOG(LogTemp, Log, TEXT("Enemy Died: %s"), *DeadEnemy->GetName());

	if (!Enemies.Remove(DeadEnemy))
	{
		UE_LOG(LogTemp, Error, TEXT("Unregistered Enemy Died: %s"), *DeadEnemy->GetName());
	}
    
	DeadEnemy->OnEnemyDeath.RemoveDynamic(this, &ARoomBase::HandleEnemyDeath);

	CheckRoomClear();
}

void ARoomBase::OpenWall(ESotaDirection Direction)
{
	// Direction은 Generator가 계산한 '월드 기준 방향'입니다.
	// 방이 회전되어 있으므로, 월드 방향을 로컬 방향으로 변환해야 합니다.
    
	UStaticMeshComponent* TargetWall = nullptr;

	// 월드 방향 벡터 구하기
	FVector WorldDir = FVector::ZeroVector;
	switch (Direction)
	{
	case ESotaDirection::Forward:  WorldDir = FVector(1, 0, 0); break;
	case ESotaDirection::Backward: WorldDir = FVector(-1, 0, 0); break;
	case ESotaDirection::Right:    WorldDir = FVector(0, 1, 0); break;
	case ESotaDirection::Left:     WorldDir = FVector(0, -1, 0); break;
	}

	// 월드 방향을 내 로컬 공간으로 역변환 (InverseTransformVector)
	FVector LocalDir = GetActorTransform().InverseTransformVectorNoScale(WorldDir);

	// 로컬 방향에 따른 벽 선택 (오차 허용 0.1)
	if (LocalDir.X > 0.5f)  TargetWall = WallForward;
	else if (LocalDir.X < -0.5f) TargetWall = WallBackward;
	else if (LocalDir.Y > 0.5f)  TargetWall = WallRight;
	else if (LocalDir.Y < -0.5f) TargetWall = WallLeft;

	if (TargetWall)
	{
		TargetWall->SetHiddenInGame(true);
		TargetWall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TargetWall->SetVisibility(false); 
	}
}