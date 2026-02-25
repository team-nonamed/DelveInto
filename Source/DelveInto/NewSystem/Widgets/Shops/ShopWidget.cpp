#include "ShopWidget.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
// UItemData가 있는 헤더 파일 경로를 프로젝트에 맞게 꼭 수정해 주세요!
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h"
#include "NewSystem/Items/ItemData.h" 
#include "NewSystem/Widgets/HandDisplayWidget.h"

void UShopWidget::SetBuyer(ADelveCharacter* Player)
{
    BuyerCharacter = Player;
}

// ==========================================================
// [신규] 키보드 입력 처리
// ==========================================================
FReply UShopWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // 누른 키가 '스페이스바(SpaceBar)' 라면 구매 실행
    if (InKeyEvent.GetKey() == EKeys::SpaceBar)
    {
        BuyPotion();
        return FReply::Handled(); // 입력을 처리했음을 엔진에 알림
    }
    // 누른 키가 'ESC(Escape)' 라면 상점 닫기 실행
    else if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        CloseShop();
        return FReply::Handled(); // 입력을 처리했음을 엔진에 알림
    }

    // 그 외의 키를 눌렀다면 기본 UI 입력 처리를 따름
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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
    		Inven->GoldAmount -= PotionPrice;
            
    		UE_LOG(LogTemp, Warning, TEXT("포션 구매 성공! 남은 금화: %d"), Inven->GoldAmount);

    		// [신규] 돈을 썼으니 메인 HUD의 골드 표시 갱신!
    		if (BuyerCharacter && BuyerCharacter->WeaponWidgetInstance)
    		{
    			BuyerCharacter->WeaponWidgetInstance->UpdateGoldDisplay(Inven->GoldAmount);
    		}
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
          // 차단했던 이동 및 시선 입력을 다시 허용합니다.
          PC->SetIgnoreMoveInput(false);
          PC->SetIgnoreLookInput(false);

          // 게임 전용 모드로 복구
          FInputModeGameOnly InputMode;
          PC->SetInputMode(InputMode);
          PC->bShowMouseCursor = false;
       }
    }
    RemoveFromParent();
}