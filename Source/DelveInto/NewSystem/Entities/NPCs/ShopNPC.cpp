#include "ShopNPC.h"
#include "Blueprint/UserWidget.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Widgets/Shops/ShopWidget.h"

AShopNPC::AShopNPC()
{
	// 메시나 콜리전 컴포넌트 추가 공간...
}

void AShopNPC::Interact_Implementation(AActor* Interactor)
{
	if (!ShopWidgetClass || !Interactor) return;

	// 상호작용한 사람이 플레이어인지 확인
	ADelveCharacter* PlayerChar = Cast<ADelveCharacter>(Interactor);
	if (!PlayerChar) return;

	APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController());
	if (!PC) return;

	// 1. 상점 위젯 생성 및 화면에 띄우기
	UShopWidget* ShopUI = CreateWidget<UShopWidget>(GetWorld(), ShopWidgetClass);
	if (ShopUI)
	{
		ShopUI->AddToViewport();
        
		// 2. 상점에게 구매자(Player) 정보 넘겨주기
		ShopUI->SetBuyer(PlayerChar);

		// 3. 마우스 커서 표시 및 UI 조작 모드로 변경
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(ShopUI->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
        
		UE_LOG(LogTemp, Display, TEXT("상점 UI를 열었습니다!"));
	}
}