// QuickSlotWidget.cpp
#include "InventoryWidget.h"

void UInventoryWidget::InitializeSlots(UInventoryHandler* InHandler)
{
	if (!InHandler) return;
    
	LinkedHandler = InHandler;
    
	// 리스트로 묶어서 인덱스로 접근하기 편하게 함
	SlotWidgets.Empty();
	SlotWidgets.Add(Slot_0);
	SlotWidgets.Add(Slot_1);
	SlotWidgets.Add(Slot_2);

	// 초기 상태 반영
	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		if (InHandler->Slots.IsValidIndex(i))
		{
			SlotWidgets[i]->UpdateSlot(InHandler->Slots[i]);
		}
	}

	// 핸들러의 이벤트 구독
	InHandler->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::OnInventoryUpdated);
}

void UInventoryWidget::OnInventoryUpdated(UItemData* ItemData, int32 AmountChanged, int32 TotalAmount)
{
	// 전체 슬롯을 순회하며 데이터 싱크 (또는 변경된 부분만 특정할 로직 필요)
	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		if (LinkedHandler->Slots.IsValidIndex(i))
		{
			SlotWidgets[i]->UpdateSlot(LinkedHandler->Slots[i]);
		}
	}
}