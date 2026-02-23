// QuickSlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.h" // 개별 슬롯 위젯을 조각으로 쓴다면 포함
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h"
#include "InventoryWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class DELVEINTO_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 인벤토리 핸들러와 연결 및 초기화
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void InitializeSlots(UInventoryHandler* InHandler);

protected:
	// 핸들러의 업데이트 이벤트를 받을 함수
	UFUNCTION()
	void OnInventoryUpdated(UItemData* ItemData, int32 AmountChanged, int32 TotalAmount);

	// 슬롯 3개를 배열로 관리 (WBP 내 위젯 이름: Slot_0, Slot_1, Slot_2)
	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot_0;

	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot_1;

	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot_2;

	UPROPERTY()
	TArray<UItemSlotWidget*> SlotWidgets;

	UPROPERTY()
	UInventoryHandler* LinkedHandler;
};