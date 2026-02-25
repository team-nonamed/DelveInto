#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// [추가] 분리한 슬롯 구조체 헤더 포함
#include "InventorySlot.h" 
#include "InventoryHandler.generated.h"

// UI 업데이트를 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryUpdatedDelegate, UItemData*, ItemData, int32, AmountChanged, int32, TotalAmount);

UCLASS(ClassGroup=(Handler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UInventoryHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryHandler();

protected:
	virtual void BeginPlay() override;

public:
	// --- 설정 ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Config")
	int32 MaxSlots = 3; // 인벤토리 슬롯 최대 개수 (1, 2, 3)

	// --- 슬롯 데이터 ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Slots")
	TArray<FInventorySlot> Slots;

	// --- 재화 데이터 (슬롯 차지 X) ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Currency")
	int32 GoldAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Currency")
	int32 MagicStoneAmount = 0;

	// 특정 아이템을 원하는 개수만큼 소모합니다. (성공하면 true 반환)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItem(UItemData* ItemToConsume, int32 Amount);

	// --- 기능 ---
	// 아이템 획득
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(UItemData* InItemData, int32 AmountToAdd);

	// 특정 슬롯의 아이템 소모
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItemAtSlot(int32 SlotIndex, int32 ConsumeAmount);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Event")
	FOnInventoryUpdatedDelegate OnInventoryUpdated;
	
};