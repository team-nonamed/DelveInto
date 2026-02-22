#include "Chest.h"
#include "PaperSpriteComponent.h"
#include "Kismet/GameplayStatics.h" // [추가] 사운드 재생 및 유틸리티
#include "NewSystem/Utils/ItemUtil.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;

	ChestSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ChestSprite"));
	RootComponent = ChestSprite;
}

void AChest::Interact_Implementation(AActor* Interactor)
{
	if (bIsOpen) return;
	bIsOpen = true;

	// 1. 시각적 연출: 뚜껑 열린 이미지로 변경
	if (OpenedSprite)
	{
		ChestSprite->SetSprite(OpenedSprite);
	}

	if (InteractSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InteractSound, GetActorLocation());
	}
	
	// 2. [추가] 청각적 연출: 상자 열리는 효과음 재생
	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}

	UE_LOG(LogTemp, Display, TEXT("보물상자를 열었습니다!"));

	// 3. 아이템 흩뿌리기
	if (LootItemID.IsValid() && LootAmount > 0)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
		UItemUtil::SpawnDroppedItems(this, LootItemID, LootAmount, SpawnLocation);
	}
}