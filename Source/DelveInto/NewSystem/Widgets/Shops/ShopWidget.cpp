#include "ShopWidget.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
// UItemData가 있는 헤더 파일 경로를 프로젝트에 맞게 꼭 수정해 주세요!
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h"
#include "NewSystem/Items/ItemData.h" 

void UShopWidget::SetBuyer(ADelveCharacter* Player)
{
	BuyerCharacter = Player;
}

void UShopWidget::BuyPotion()
{
	if (!BuyerCharacter || !BuyerCharacter->InventoryHandler) return;

	if (!PotionItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("ShopWidget에 PotionItemData가 설정되지 않았습니다! (블루프린트 확인)"));
		return;
	}

	UInventoryHandler* Inven = BuyerCharacter->InventoryHandler;

	// 1. 플레이어의 돈(Gold)이 충분한지 확인
	if (Inven->GoldAmount >= PotionPrice)
	{
		// 2. 돈을 빼기 전에, 인벤토리에 포션을 먼저 넣어봅니다.
		// AddItem은 다 못 넣고 남은 개수를 반환합니다.
		int32 Leftover = Inven->AddItem(PotionItemData, 1);

		// 3. Leftover가 0이라면 1개가 인벤토리에 무사히 다 들어갔다는 뜻!
		if (Leftover == 0)
		{
			// 4. 아이템이 안전하게 들어갔으니 금화를 차감합니다.
			Inven->GoldAmount -= PotionPrice;
            
			UE_LOG(LogTemp, Warning, TEXT("포션 구매 성공! 남은 금화: %d"), Inven->GoldAmount);

			// (선택 사항) 금화가 깎인 것을 UI에 알리기 위해 델리게이트 강제 호출
			// Inven->OnInventoryUpdated.Broadcast(nullptr, -PotionPrice, Inven->GoldAmount);
		}
		else
		{
			// 인벤토리가 꽉 찼다면 AddItem 내부에서 처리가 거부되었으므로 돈을 빼지 않습니다.
			UE_LOG(LogTemp, Warning, TEXT("인벤토리가 가득 차서 구매할 수 없습니다!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("금화가 부족합니다. (현재: %d / 필요: %d)"), Inven->GoldAmount, PotionPrice);
	}
}

void UShopWidget::CloseShop()
{
	if (BuyerCharacter)
	{
		APlayerController* PC = Cast<APlayerController>(BuyerCharacter->GetController());
		if (PC)
		{
			// 게임 전용 모드로 복구 및 마우스 숨김
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
	}

	// 위젯 파괴
	RemoveFromParent();
}