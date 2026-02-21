#include "ItemInstance.h"
#include "Components/SphereComponent.h"
#include "PaperSpriteComponent.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h"

AItemInstance::AItemInstance()
{
	PrimaryActorTick.bCanEverTick = false;

	// 충돌체 설정
	PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollider"));
	RootComponent = PickupCollider;
	PickupCollider->InitSphereRadius(50.0f);
	PickupCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemInstance::OnBeginOverlap);

	// 시각적 표현
	ItemSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ItemSprite"));
	ItemSprite->SetupAttachment(RootComponent);
	ItemSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
}

void AItemInstance::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ItemData가 할당되지 않은 오류 상태라면 무시
	if (!ItemData) return;

	if (OtherActor && OtherActor->IsA(ADelveCharacter::StaticClass()))
	{
		ADelveCharacter* PlayerChar = Cast<ADelveCharacter>(OtherActor);
		UInventoryHandler* InvHandler = PlayerChar->FindComponentByClass<UInventoryHandler>();
        
		if (InvHandler)
		{
			// 1. 인벤토리에 아이템 추가를 시도하고, 다 못 넣고 남은 개수를 반환받습니다.
			int32 LeftoverAmount = InvHandler->AddItem(ItemData, Amount);
            
			// 2. 남은 개수가 없다면 (전부 획득했다면) 액터를 파괴합니다.
			if (LeftoverAmount <= 0)
			{
				// TODO: 획득 효과음/이펙트 재생 로직 추가
				Destroy();
			}
			// 3. 인벤토리가 꽉 차서 일부를 남겼다면, 떨어진 개수를 갱신합니다.
			else
			{
				Amount = LeftoverAmount;
				UE_LOG(LogTemp, Warning, TEXT("인벤토리 공간 부족으로 %d개를 남겼습니다."), Amount);
			}
		}
	}
}

void AItemInstance::InitializeItem(UItemData* InItemData, int32 InAmount)
{
	// 1. 전달받은 데이터와 수량을 내 변수에 저장
	ItemData = InItemData;
	Amount = InAmount;

	// 2. 시각적 업데이트: 할당된 아이템 데이터에 맞게 외형을 바꿉니다!
	if (ItemData && ItemSprite)
	{
		// 💡 팁: UItemData.h에서 ItemIcon 변수의 타입을 UTexture2D* 대신 
		// UPaperSprite* 로 선언해두셨다면 아래처럼 곧바로 적용할 수 있습니다.
        
		/*
		if (ItemData->ItemIconSprite)
		{
			ItemSprite->SetSprite(ItemData->ItemIconSprite);
		}
		*/

		UE_LOG(LogTemp, Display, TEXT("%s 아이템이 %d개 스폰되었습니다!"), *ItemData->ItemName.ToString(), Amount);
	}
}