#include "TutorialWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 위젯이 키보드/마우스 입력을 받을 수 있도록 포커스 설정
	this->SetIsFocusable(true);
}

void UTutorialWidget::ShowTutorialPrompt(FText Message, FKey KeyToWait)
{
	if (PromptText) PromptText->SetText(Message);
	RequiredKey = KeyToWait;

	SetVisibility(ESlateVisibility::Visible);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.001f);

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		// [핵심] 입력을 게임으로 100% 통과시킵니다. UIOnly나 포커스 지정이 필요 없습니다!
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(true);
		PC->SetInputMode(InputMode);
	}
}

// [추가된 감시 로직]
void UTutorialWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 위젯이 화면에 떠 있고, 기다리는 키가 설정되어 있을 때
	if (RequiredKey.IsValid() && GetVisibility() == ESlateVisibility::Visible)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			// 플레이어가 해당 키를 누르는 순간!
			if (PC->IsInputKeyDown(RequiredKey))
			{
				CompletePrompt();
			}
		}
	}
}

void UTutorialWidget::CompletePrompt()
{
	RequiredKey = FKey(); // 감시할 키 초기화
	SetVisibility(ESlateVisibility::Collapsed);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	if (OnStepCompleted.IsBound())
	{
		OnStepCompleted.Broadcast();
	}
}