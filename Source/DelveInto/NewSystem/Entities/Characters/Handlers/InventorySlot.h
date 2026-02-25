#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Items/ItemData.h"

#include "InventorySlot.generated.h"

/**
 * 인벤토리의 한 '칸(Slot)'을 나타내는 구조체
 */
USTRUCT(BlueprintType)
struct DELVEINTO_API FInventorySlot
{
	GENERATED_BODY()

	// 이 슬롯에 들어있는 아이템 데이터 (비어있으면 nullptr)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	UItemData* ItemData = nullptr;

	// 현재 슬롯에 들어있는 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 Amount = 0;

	// 슬롯이 비어있는지 확인하는 헬퍼 함수
	bool IsEmpty() const 
	{ 
		return ItemData == nullptr || Amount <= 0; 
	}

	// 슬롯 비우기
	void Clear()
	{
		ItemData = nullptr;
		Amount = 0;
	}
};