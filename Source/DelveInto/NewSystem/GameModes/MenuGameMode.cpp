#include "MenuGameMode.h"
#include "Blueprint/UserWidget.h"

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		// 1. 마우스 커서 보이기
		PC->bShowMouseCursor = true;
        
		// 2. UI 조작 모드로 설정
		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	// 3. 타이틀 위젯 생성 및 부착
	if (MainMenuWidgetClass)
	{
		UUserWidget* MenuUI = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (MenuUI)
		{
			MenuUI->AddToViewport();
		}
	}
}