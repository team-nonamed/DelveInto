#include "DelveRoomBase.h"
#include "Components/BoxComponent.h"
#include "Dungeons/Props/Connectors/DelveDoor.h"
#include "Entities/Characters/DelveCharacter.h"

ADelveRoomBase::ADelveRoomBase()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	// 방 크기만큼 트리거 설정 (예: 2000x2000)
	RoomTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomTrigger"));
	RoomTrigger->SetupAttachment(RootComponent);
	RoomTrigger->SetBoxExtent(FVector(900.f, 900.f, 500.f)); 
    
	// 이벤트 바인딩은 BeginPlay나 PostInitializeComponents에서 하는게 안전
}

void ADelveRoomBase::BeginPlay()
{
	Super::BeginPlay();
	RoomTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADelveRoomBase::OnTriggerOverlap);
}

void ADelveRoomBase::OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasEntered) return; // 이미 방문했으면 무시 (전투가 끝난 방)

	if (OtherActor && OtherActor->IsA(ADelveCharacter::StaticClass()))
	{
		bHasEntered = true;
		OnPlayerEnterRoom(); // [핵심] 자식 클래스의 로직 실행
	}
}

// [가상 함수] 기본 동작: 전투방이 아니면 바로 클리어 처리
void ADelveRoomBase::OnPlayerEnterRoom()
{
	// 기본(Normal) 방의 경우 몬스터가 없으면 바로 오픈
	// 자식 클래스(BattleRoom)에서는 이 함수를 덮어써서 문을 잠글 것임
	if (RoomType == ERoomType::Normal)
	{
		UnlockDoors();
	}
}

void ADelveRoomBase::CheckRoomCleared()
{
	// 자식 클래스에서 몬스터 수를 세서 0이면 호출
	bIsCleared = true;
	UnlockDoors();
}

void ADelveRoomBase::LockDoors()
{
	for (ADelveDoor* Door : SpawnedDoors)
	{
		if(Door) Door->CloseConnector();
	}
}

void ADelveRoomBase::UnlockDoors()
{
	for (ADelveDoor* Door : SpawnedDoors)
	{
		if(Door) Door->OpenConnector();
	}
}