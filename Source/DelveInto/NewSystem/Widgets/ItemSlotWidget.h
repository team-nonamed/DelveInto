#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewSystem/Entities/Characters/Handlers/InventorySlot.h"
#include "ItemSlotWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * 개별 아이템 슬롯을 표시하는 위젯 조각
 */
UCLASS(Abstract, Blueprintable)
class DELVEINTO_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부(부모 위젯)에서 이 슬롯의 데이터를 설정할 때 호출
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void UpdateSlot(const FInventorySlot& NewSlotData);

	// 슬롯 초기화 (빈 상태로 만들기)
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void ClearSlot();

protected:
	// --- UI 바인딩 (WBP의 위젯 이름과 일치해야 함) ---
    
	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackText;

	// --- 설정 ---
    
	// 아이템이 없을 때 보여줄 기본 아이콘 (선택 사항)
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	UTexture2D* EmptySlotTexture;
};