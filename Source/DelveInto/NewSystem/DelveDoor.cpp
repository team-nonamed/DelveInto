#include "DelveDoor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ADelveDoor::ADelveDoor()
{
    PrimaryActorTick.bCanEverTick = false; // 문은 틱이 필요 없음 (최적화)

    // 1. 루트 컴포넌트
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // 2. 문틀 (충돌은 Mesh 자체 설정을 따름)
    DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
    DoorFrame->SetupAttachment(RootComponent);

    // // 3. 문짝 (실제로 길을 막는 비주얼)
    // DoorPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanel"));
    // DoorPanel->SetupAttachment(RootComponent);
    // // 문짝 위치를 문틀 안쪽으로 살짝 조정 (필요시)
    // DoorPanel->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); 

    // 4. 블로커 (플레이어 이동 차단용 투명벽)
    BlockerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockerCollision"));
    BlockerCollision->SetupAttachment(RootComponent);
    BlockerCollision->SetBoxExtent(FVector(50.f, 100.f, 100.f)); // 크기 적절히 조절
    
    // 초기 설정: 닫힌 상태라면 블로커 켜기, 열린 상태라면 끄기
    // (생성자에서는 기본값만 설정하고, 실제 로직은 BeginPlay나 함수에서)
    BlockerCollision->SetCollisionProfileName(TEXT("BlockAll"));
}

void ADelveDoor::BeginPlay()
{
    Super::BeginPlay();

    // 시작 상태에 맞춰 문 설정
    if (bIsOpen)
    {
        UnlockAndOpen(false);
    }
    else
    {
        CloseAndLock();
    }
}

void ADelveDoor::UnlockAndOpen(bool Sound)
{
    if (bIsOpen) return;
    bIsOpen = true;

    // 1. 충돌체 끄기 (C++에서 처리)
    if (BlockerCollision) BlockerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 2. [핵심] 블루프린트에 "문 열어라!" 신호 보냄
    // 이제 BP에 있는 'Event Open Door'가 실행되면서 타임라인이 돌기 시작합니다.
    OpenDoor(Sound); 
}

void ADelveDoor::CloseAndLock()
{
    if (!bIsOpen) return;
    bIsOpen = false;

    // 1. 충돌체 켜기
    if (BlockerCollision) BlockerCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 2. 블루프린트 신호
    CloseDoor();
}