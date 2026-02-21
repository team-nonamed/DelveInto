#include "ItemUtil.h"
#include "Engine/GameInstance.h"
#include "NewSystem/Items/ItemInstance.h"
#include "NewSystem/Items/Databases/ItemDBSubsystem.h"

AItemInstance* UItemUtil::SpawnDroppedItem(const UObject* WorldContextObject, FGameplayTag ItemIDToSpawn, int32 Amount, FVector SpawnLocation)
{
	// 태그가 유효하지 않으면 곧바로 취소
	if (!WorldContextObject || !ItemIDToSpawn.IsValid()) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	// 1. 게임 인스턴스에서 아이템 DB 서브시스템을 가져옵니다.
	UItemDBSubsystem* ItemDB = World->GetGameInstance()->GetSubsystem<UItemDBSubsystem>();
	if (!ItemDB) return nullptr;

	// 2. DB에 아이템 정보를 물어봅니다.
	FItemDBRow ItemInfo;
	if (!ItemDB->GetItemInfo(ItemIDToSpawn, ItemInfo))
	{
		// ToString()을 호출하면 로그에 "Item.ID.Gold" 처럼 예쁘게 찍힙니다.
		UE_LOG(LogTemp, Error, TEXT("ItemDB에 해당 ID(%s)의 아이템 정보가 없습니다!"), *ItemIDToSpawn.ToString());
		return nullptr;
	}

	// 3. 스폰 진행
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AItemInstance* SpawnedItem = World->SpawnActor<AItemInstance>(
		ItemInfo.ItemInstanceClass, 
		SpawnLocation, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	// 4. 데이터 주입
	if (SpawnedItem)
	{
		SpawnedItem->InitializeItem(ItemInfo.ItemData, Amount);
	}

	return SpawnedItem;
}
