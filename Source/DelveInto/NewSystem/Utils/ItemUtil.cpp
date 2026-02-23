#include "ItemUtil.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Math/UnrealMathUtility.h"
#include "NewSystem/Items/ItemInstance.h"
#include "NewSystem/Items/Databases/ItemDBSubsystem.h"

TArray<AItemInstance*> UItemUtil::SpawnDroppedItems(const UObject* WorldContextObject, FGameplayTag ItemIDToSpawn, int32 Amount, FVector SpawnLocation, FVector LaunchImpulse)
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
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 i = 0; i < Amount; ++i)
    {
        // 1. 약간의 랜덤 위치 편차 (아이템들이 겹쳐서 스폰되지 않게 함)
        FVector RandomPos = FVector(FMath::RandRange(-10.f, 10.f), FMath::RandRange(-10.f, 10.f), 0.f);
        
        AItemInstance* SpawnedItem = World->SpawnActor<AItemInstance>(
            AItemInstance::StaticClass(), 
            SpawnLocation + RandomPos, 
            FRotator::ZeroRotator, 
            SpawnParams
        );

        if (SpawnedItem)
        {
            SpawnedItem->InitializeItem(ItemInfo.ItemData, 1);
            SpawnedItems.Add(SpawnedItem);

            // 2. [역동적 연출] 물리 컴포넌트에 힘 가하기
            // AItemInstance의 루트가 PrimitiveComponent(Mesh, Collision 등)라고 가정합니다.
            if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(SpawnedItem->GetRootComponent()))
            {
                // 물리 엔진 활성화
                RootPrim->SetSimulatePhysics(true);
                RootPrim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

                // 3. 개별 아이템마다 약간의 랜덤한 방향성 추가 (더 자연스럽게 퍼짐)
                FVector RandomSpread = FVector(FMath::RandRange(-0.3f, 0.3f), FMath::RandRange(-0.3f, 0.3f), FMath::RandRange(0.f, 0.2f));
                FVector FinalImpulse = (LaunchImpulse + (LaunchImpulse.Size() * RandomSpread));

                // 즉각적인 힘(Impulse) 적용
                RootPrim->AddImpulse(FinalImpulse, NAME_None, true);
            }
        }
    }

    return SpawnedItems;
}