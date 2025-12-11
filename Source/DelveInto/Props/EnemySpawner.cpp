#include "EnemySpawner.h"
#include "RoomBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    // OwningRoom을 BP에서 안 채웠으면, 부모 액터에서 캐스팅해보는 식으로 자동 추론도 가능
    if (!OwningRoom)
    {
        if (AActor* Parent = GetAttachParentActor())
        {
            OwningRoom = Cast<ARoomBase>(Parent);
        }
    }

    if (OwningRoom)
    {
        // 이벤트 구독 (C#의 room.OnPlayerEnter += Handler 느낌)
        OwningRoom->OnPlayerEnterRoom.AddDynamic(
            this,
            &AEnemySpawner::HandleOnPlayerEnterRoom
        );
    }
}

void AEnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (OwningRoom)
    {
        // 파괴될 때 언바인딩 (안 해도 큰 문제는 없지만 깔끔하게)
        OwningRoom->OnPlayerEnterRoom.RemoveDynamic(
            this,
            &AEnemySpawner::HandleOnPlayerEnterRoom
        );
    }

    Super::EndPlay(EndPlayReason);
}

void AEnemySpawner::HandleOnPlayerEnterRoom(AFPSPlayer* PlayerActor)
{
    // 여기서 조건 걸고, 실제 스폰 호출
    if (!bHasSpawned)
    {
        UE_LOG(LogTemp, Error, TEXT("Player spawned"));
        SpawnEnemiesInternal(PlayerActor);
        bHasSpawned = true;
    }
}

void AEnemySpawner::SpawnEnemiesInternal(AFPSPlayer* PlayerActor)
{
    if (!EnemyClass || SpawnCount <= 0)
        return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    for (int32 i = 0; i < SpawnCount; ++i)
    {
        const float Angle = (2.f * PI / FMath::Max(SpawnCount, 1)) * i;
        const FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * SpawnRadius;

        const FVector SpawnLocation = GetActorLocation() + Offset;
        const FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        World->SpawnActor<APawn>(
            EnemyClass,
            SpawnLocation,
            SpawnRotation,
            Params
        );
    }
}
