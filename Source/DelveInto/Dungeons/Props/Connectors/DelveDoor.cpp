#include "DelveDoor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ADelveDoor::ADelveDoor()
{
    PrimaryActorTick.bCanEverTick = false; 

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Center Anchor"));
    RootComponent = RootScene;

    DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Frame"));
    DoorFrame->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    DoorFrame->SetupAttachment(RootComponent);

    // [수정 3 적용] DoorPanel 재생성
    DoorPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Panel"));
    DoorPanel->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    DoorPanel->SetupAttachment(RootComponent);

    // BlockerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Blocker Collision"));
    // BlockerCollision->SetupAttachment(RootComponent);
    // BlockerCollision->SetBoxExtent(FVector(50.f, 100.f, 100.f)); 
    // BlockerCollision->SetCollisionProfileName(TEXT("BlockAll"));
}

void ADelveDoor::BeginPlay()
{
    Super::BeginPlay();
    
    bIsOpen = DoorConfig.bIsOpenedInClass;

    if (bIsOpen == DoorConfig.bIsOpenedInitially)
    {
        return;
    }
    
    if (DoorConfig.bIsOpenedInitially)
    {
        OpenConnector(false);
    }
    else
    {
        CloseConnector(false);
    }
}

// 기존 UnlockAndOpen을 OpenConnector로 변경
void ADelveDoor::OpenConnector(bool bPlaySound)
{
    if (bIsOpen) return;
    bIsOpen = true;

    //if (BlockerCollision) BlockerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    OpenDoor(bPlaySound); 
}

// 기존 CloseAndLock을 CloseConnector로 변경
void ADelveDoor::CloseConnector(bool bPlaySound)
{
    if (!bIsOpen) return;
    bIsOpen = false;

    //if (BlockerCollision) BlockerCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    CloseDoor(bPlaySound);
}