#include "SpawnEnemyTriggerBox.h"

#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Dungeons/Props/Rooms/RoomBase.h"

class ARoomBase;

USpawnEnemyTriggerBox::USpawnEnemyTriggerBox()
{
	// 기본 설정
	PrimaryComponentTick.bCanEverTick = false;
    
	// 콜리전 프리셋 설정 (Trigger)
	SetCollisionProfileName(TEXT("Trigger"));
    
	// 박스 크기 기본값 (필요시 RoomBase에서 덮어씀)
	InitBoxExtent(FVector(100.0f, 100.0f, 100.0f));
}

void USpawnEnemyTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	// 주인 검사를 통과한 경우에만 바인딩
	if (GetOwner() && GetOwner()->IsA(ARoomBase::StaticClass()))
	{
		OnComponentBeginOverlap.AddDynamic(this, &USpawnEnemyTriggerBox::OnOverlapBegin);
	}
}

void USpawnEnemyTriggerBox::OnRegister()
{
	Super::OnRegister();

	// 1. 주인이 있는지 확인
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	// 2. 주인이 ARoomBase(혹은 그 자식)인지 확인
	// IsA는 상속 관계까지 체크해주므로 가장 정확합니다.
	if (!MyOwner->IsA(ARoomBase::StaticClass()))
	{
		// [에러 처리] RoomBase가 아니라면?
        
#if WITH_EDITOR
		// 에디터라면 빨간색 로그를 띄워서 개발자에게 경고
		FString ErrorMsg = FString::Printf(TEXT("ERROR: 'SpawnTriggerBox' must be attached to 'ARoomBase'! Current Owner: %s"), *MyOwner->GetName());
        
		// 화면에 로그 출력 (Key를 지정해서 도배 방지)
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage((uint64)this, 10.0f, FColor::Red, ErrorMsg);
		}
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
#endif

		// 3. 기능 정지 (충돌 끄기 & 틱 끄기)
		// 스스로 DestroyComponent()를 하면 크래시 위험이 있으므로 기능을 끄는 게 안전함
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PrimaryComponentTick.bCanEverTick = false;
	}
}

void USpawnEnemyTriggerBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. 플레이어인지 확인
	ADelveCharacter* Character = Cast<ADelveCharacter>(OtherActor);
	if (!Character) return;

	// 2. 이 컴포넌트의 주인(Owner)이 RoomBase인지 확인
	ARoomBase* OwnerRoom = Cast<ARoomBase>(GetOwner());
	if (OwnerRoom)
	{
		// 3. 방의 스폰 함수 호출
		OwnerRoom->TrigSpawn();

		// 4. 역할 끝났으니 비활성화 (최적화)
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UE_LOG(LogTemp, Log, TEXT("SpawnTriggerBox: Triggered Spawn for %s"), *OwnerRoom->GetName());
	}
}