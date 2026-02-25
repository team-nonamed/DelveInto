// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomBase.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NewSystem/DelveDoor.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"


// Sets default values
ARoomBase::ARoomBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene"));
	RootComponent = RootScene;

#if WITH_EDITOR
	// =========================================================================
	// 정리용 홀더(SceneComponent) 생성
	// =========================================================================
    
	// ArrowHolder 생성
	ArrowHolder = CreateDefaultSubobject<USceneComponent>(TEXT("Arrow Holder"));
	ArrowHolder->SetupAttachment(RootComponent); // 루트에 붙임
	ArrowHolder->SetHiddenInGame(true);
	ArrowHolder->bIsEditorOnly = true;           // 빌드 시 삭제

	// BoxHolder 생성
	BoxHolder = CreateDefaultSubobject<USceneComponent>(TEXT("Box Holder"));
	BoxHolder->SetupAttachment(RootComponent);   // 루트에 붙임
	BoxHolder->SetHiddenInGame(true);
	BoxHolder->bIsEditorOnly = true;             // 빌드 시 삭제
	
	// 1. 박스 컴포넌트 (Box)
	DebugFloorRegion = CreateDefaultSubobject<UBoxComponent>(TEXT("Debug Floor Region"));
	DebugFloorRegion->SetupAttachment(BoxHolder);
	
	DebugFloorRegion->SetCollisionProfileName(TEXT("NoCollision")); // 충돌 없음
	DebugFloorRegion->SetGenerateOverlapEvents(false);
	DebugFloorRegion->SetHiddenInGame(true);                        // 게임에선 숨김
	DebugFloorRegion->bIsEditorOnly = true;
	DebugFloorRegion->ShapeColor = FColor::Cyan;
	DebugFloorRegion->SetLineThickness(10.0f);

	// 2. [신규] 중앙 화살표 (위를 바라봄)
	ArrowCenter = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Center"));
	ArrowCenter->SetupAttachment(ArrowHolder);
	ArrowCenter->SetHiddenInGame(true);
	ArrowCenter->bIsEditorOnly = true;
	ArrowCenter->ArrowColor = FColor::White;
	ArrowCenter->ArrowSize = 10.0f; // 잘 보이게 크게
	// 기본적으로 Arrow는 X축(Forward)을 보므로, 위(Z축)를 보게 하려면 Pitch를 90도 회전
	ArrowCenter->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	// 2. 화살표 생성 도우미 람다 함수 (반복 코드 줄이기)
	auto CreateVisArrow = [&](FName Name, FColor Color) -> UArrowComponent*
	{
		UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(Name);
		Arrow->SetupAttachment(ArrowHolder);

		Arrow->SetCollisionProfileName(TEXT("NoCollision"));
		Arrow->SetHiddenInGame(true); // 게임에선 숨김
		Arrow->bIsEditorOnly = true;
		
		Arrow->ArrowColor = Color;
		Arrow->ArrowSize = 10.0f;
		return Arrow;
	};
	
	// 모서리 (노랑) - 대각선 방향
	ArrowFrontRight = CreateVisArrow(TEXT("Arrow Forward Right"), FColor::Yellow);
	ArrowFrontLeft  = CreateVisArrow(TEXT("Arrow Forward Left"), FColor::Yellow);
	ArrowBackRight  = CreateVisArrow(TEXT("Arrow Backward Right"), FColor::Yellow);
	ArrowBackLeft   = CreateVisArrow(TEXT("Arrow Backward Left"), FColor::Yellow);

	// 3. [신규] 문 위치 디버그 박스 생성 헬퍼 람다
	auto CreateDoorBox = [&](FName Name, FColor Color) -> UBoxComponent*
	{
		UBoxComponent* Box = CreateDefaultSubobject<UBoxComponent>(Name);
		Box->SetupAttachment(BoxHolder);
        
		Box->SetCollisionProfileName(TEXT("NoCollision"));
		Box->SetGenerateOverlapEvents(false);
		Box->SetHiddenInGame(true);
		Box->bIsEditorOnly = true;
        
		Box->ShapeColor = Color;
		Box->SetLineThickness(10.0f);
		return Box;
	};

	// 문 박스 생성 (색상은 Magenta로 통일하거나 구분 가능)
	DebugDoorForward  = CreateDoorBox(TEXT("Debug Door Forward"), FColor::Magenta);
	DebugDoorBackward = CreateDoorBox(TEXT("Debug Door Backward"), FColor::Emerald);
	DebugDoorRight    = CreateDoorBox(TEXT("Debug Door Right"), FColor::Emerald);
	DebugDoorLeft     = CreateDoorBox(TEXT("Debug Door Left"), FColor::Emerald);
	
#endif
}

void ARoomBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

	WallForward = Cast<UStaticMeshComponent>(WallForwardRef.GetComponent(this));
	WallBackward = Cast<UStaticMeshComponent>(WallBackwardRef.GetComponent(this));
	WallRight = Cast<UStaticMeshComponent>(WallRightRef.GetComponent(this));
	WallLeft = Cast<UStaticMeshComponent>(WallLeftRef.GetComponent(this));

#if WITH_EDITOR
    // =========================================================================
    // Size 변수가 바뀔 때마다 박스와 화살표 위치 자동 갱신
    // =========================================================================
    
    // 1. 박스 크기 업데이트
    // Size는 전체 길이이므로 BoxExtent(반지름 개념)에는 절반을 넣습니다. 높이는 100으로 고정.
    float HalfSize = Size * 0.5f;
    if (DebugFloorRegion)
    {
        DebugFloorRegion->SetBoxExtent(FVector(HalfSize, HalfSize, 50.0f));
    	// [핵심] 윗면이 0이 되려면, 중심을 두께 절반만큼 아래로 내려야 함
    	// 범위: -100 ~ 0 (윗면이 0)
    	DebugFloorRegion->SetRelativeLocation(FVector(0.0f, 0.0f, -DebugFloorRegion->GetScaledBoxExtent().Z));
    }

	// 2. 문 박스 치수 설정
	// 요구사항: 높이 700, 가로 700, 깊이 50
	// SetBoxExtent는 '반지름' 개념이므로 절반 값을 넣어야 함
	// 높이(Z) 절반: 350 / 가로(Y) 절반: 350 / 깊이(X) 절반: 25
    
	float DoorHalfHeight = 350.0f;
	float DoorHalfWidth = 350.0f;
	float DoorHalfDepth = 25.0f;

	float DoorZ = DoorHalfHeight;

	// 3. 각 방향별 문 박스 배치
	if (DebugDoorForward)
	{
		// 위치: X는 앞으로 전진, Y는 중앙, Z는 바닥 위
		DebugDoorForward->SetRelativeLocation(FVector(HalfSize, 0.0f, DoorZ));
		// 크기: X가 깊이(두께), Y가 너비, Z가 높이
		DebugDoorForward->SetBoxExtent(FVector(DoorHalfDepth, DoorHalfWidth, DoorHalfHeight));
	}

	if (DebugDoorBackward)
	{
		DebugDoorBackward->SetRelativeLocation(FVector(-HalfSize, 0.0f, DoorZ));
		DebugDoorBackward->SetBoxExtent(FVector(DoorHalfDepth, DoorHalfWidth, DoorHalfHeight));
	}

	if (DebugDoorRight)
	{
		DebugDoorRight->SetRelativeLocation(FVector(0.0f, HalfSize, DoorZ));
		// 오른쪽 문은 회전되어 있으므로 X가 너비, Y가 깊이가 됨 (또는 회전값 적용)
		// 여기서는 회전 없이 Extent를 바꿔서 표현
		DebugDoorRight->SetBoxExtent(FVector(DoorHalfWidth, DoorHalfDepth, DoorHalfHeight));
	}

	if (DebugDoorLeft)
	{
		DebugDoorLeft->SetRelativeLocation(FVector(0.0f, -HalfSize, DoorZ));
		DebugDoorLeft->SetBoxExtent(FVector(DoorHalfWidth, DoorHalfDepth, DoorHalfHeight));
	}


    // 3. 모서리 화살표 위치 및 회전 (바깥쪽을 바라보게)
    if (ArrowFrontRight)
    {
        ArrowFrontRight->SetRelativeLocation(FVector(HalfSize, HalfSize, 0.0f));
        ArrowFrontRight->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
    }
    if (ArrowFrontLeft)
    {
        ArrowFrontLeft->SetRelativeLocation(FVector(HalfSize, -HalfSize, 0.0f));
        ArrowFrontLeft->SetRelativeRotation(FRotator(0.0f, -45.0f, 0.0f));
    }
    if (ArrowBackRight)
    {
        ArrowBackRight->SetRelativeLocation(FVector(-HalfSize, HalfSize, 0.0f));
        ArrowBackRight->SetRelativeRotation(FRotator(0.0f, 135.0f, 0.0f));
    }
    if (ArrowBackLeft)
    {
        ArrowBackLeft->SetRelativeLocation(FVector(-HalfSize, -HalfSize, 0.0f));
        ArrowBackLeft->SetRelativeRotation(FRotator(0.0f, -135.0f, 0.0f));
    }
#endif
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

void ARoomBase::OpenWall(ERoomDirection Direction)
{
	// Direction은 Generator가 계산한 '월드 기준 방향'입니다.
	// 방이 회전되어 있으므로, 월드 방향을 로컬 방향으로 변환해야 합니다.
    
	UStaticMeshComponent* TargetWall = nullptr;

	// 월드 방향 벡터 구하기
	FVector WorldDir = FVector::ZeroVector;
	switch (Direction)
	{
	case ERoomDirection::Forward:  WorldDir = FVector(1, 0, 0); break;
	case ERoomDirection::Backward: WorldDir = FVector(-1, 0, 0); break;
	case ERoomDirection::Right:    WorldDir = FVector(0, 1, 0); break;
	case ERoomDirection::Left:     WorldDir = FVector(0, -1, 0); break;
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