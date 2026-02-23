#include "ItemSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PaperSprite.h" // Paper2D 모듈 필요

void UItemSlotWidget::UpdateSlot(const FInventorySlot& NewSlotData)
{
	if (NewSlotData.IsEmpty())
	{
		ClearSlot();
		return;
	}

	// 1. 아이콘 설정 (ItemData 내의 UPaperSprite 사용)
	if (ItemIcon && NewSlotData.ItemData->ItemIcon)
	{
		// PaperSprite를 Image 위젯에 적용
		ItemIcon->SetBrushFromAtlasInterface(NewSlotData.ItemData->ItemIcon);
		ItemIcon->SetRenderOpacity(1.0f);
	}

	// 2. 개수 텍스트 설정
	if (StackText)
	{
		// 중첩 가능한 아이템이고 1개보다 많을 때만 숫자 표시 (보통의 UX)
		if (NewSlotData.ItemData->MaxStack > 1)
		{
			StackText->SetText(FText::AsNumber(NewSlotData.Amount));
			StackText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StackText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UItemSlotWidget::ClearSlot()
{
	if (ItemIcon)
	{
		if (EmptySlotTexture)
		{
			ItemIcon->SetBrushFromTexture(EmptySlotTexture);
			ItemIcon->SetRenderOpacity(0.3f); // 빈 슬롯은 투명하게 표현하거나
		}
		else
		{
			ItemIcon->SetRenderOpacity(0.0f); // 아예 안 보이게 처리
		}
	}

	if (StackText)
	{
		StackText->SetVisibility(ESlateVisibility::Collapsed);
	}
}