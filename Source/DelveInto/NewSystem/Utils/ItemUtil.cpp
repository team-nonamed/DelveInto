#include "ItemUtil.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Math/UnrealMathUtility.h"
#include "NewSystem/Items/ItemInstance.h"
#include "NewSystem/Items/Databases/ItemDBSubsystem.h"

TArray<AItemInstance*> UItemUtil::SpawnDroppedItems(const UObject* WorldContextObject, FGameplayTag ItemIDToSpawn, int32 Amount, FVector SpawnLocation)
{
    TArray<AItemInstance*> SpawnedItems;

    if (!WorldContextObject || !ItemIDToSpawn.IsValid() || Amount <= 0) return SpawnedItems;

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return SpawnedItems;

    UItemDBSubsystem* ItemDB = World->GetGameInstance()->GetSubsystem<UItemDBSubsystem>();
    if (!ItemDB) return SpawnedItems;

    FItemDBRow ItemInfo;
    if (!ItemDB->GetItemInfo(ItemIDToSpawn, ItemInfo)) return SpawnedItems;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    for (int32 i = 0; i < Amount; ++i)
    {
        FVector RandomOffset = FVector(
            FMath::RandRange(-50.0f, 50.0f), 
            FMath::RandRange(-50.0f, 50.0f), 
            FMath::RandRange(10.0f, 40.0f)
        );
        FVector FinalSpawnLocation = SpawnLocation + RandomOffset;

        // [핵심] 순수 C++ 클래스를 다이렉트로 스폰합니다!
        AItemInstance* SpawnedItem = World->SpawnActor<AItemInstance>(
            AItemInstance::StaticClass(), 
            FinalSpawnLocation, 
            FRotator::ZeroRotator, 
            SpawnParams
        );

        if (SpawnedItem)
        {
            SpawnedItem->InitializeItem(ItemInfo.ItemData, 1);
            SpawnedItems.Add(SpawnedItem);
        }
    }

    return SpawnedItems;
}