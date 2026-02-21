#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h" // [필수]
#include "NewSystem/Items/ItemData.h"
#include "ItemDBRow.generated.h"

class AItemInstance;

USTRUCT(BlueprintType)
struct DELVEINTO_API FItemDBRow : public FTableRowBase
{
	GENERATED_BODY()

	// 1. 아이템 고유 식별자 (예: Item.ID.Gold, Item.ID.HealthPotion)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item DB")
	FGameplayTag ItemID;

	// 2. 이 아이템에 해당하는 실제 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item DB")
	UItemData* ItemData = nullptr;

	// 3. 바닥에 스폰할 때 쓸 블루프린트 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item DB")
	TSubclassOf<AItemInstance> ItemInstanceClass;
};