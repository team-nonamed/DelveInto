#include "DelveDoor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ADelveDoor::ADelveDoor()
{
    PrimaryActorTick.bCanEverTick = false; 

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
    DoorFrame->SetupAttachment(RootComponent);

    // [수정 3 적용] DoorPanel 재생성
    DoorPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanel"));
    DoorPanel->SetupAttachment(RootComponent);

    BlockerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockerCollision"));
    BlockerCollision->SetupAttachment(RootComponent);
    BlockerCollision->SetBoxExtent(FVector(50.f, 100.f, 100.f)); 
    BlockerCollision->SetCollisionProfileName(TEXT("BlockAll"));
}

void ADelveDoor::BeginPlay()
{
    Super::BeginPlay();

    if (bIsOpen)
    {
        OpenConnector(false);
    }
    else
    {
        CloseConnector();
    }
}

// 기존 UnlockAndOpen을 OpenConnector로 변경
void ADelveDoor::OpenConnector(bool bPlaySound)
{
    if (bIsOpen) return;
    bIsOpen = true;

    if (BlockerCollision) BlockerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    OpenDoor(bPlaySound); 
}

// 기존 CloseAndLock을 CloseConnector로 변경
void ADelveDoor::CloseConnector()
{
    if (!bIsOpen) return;
    bIsOpen = false;

    if (BlockerCollision) BlockerCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    CloseDoor();
}