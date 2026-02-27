#include "TutorialWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Input/Reply.h"

void UTutorialWidget::NativeConstruct()
{
    Super::NativeConstruct();
    this->SetIsFocusable(true);
}

void UTutorialWidget::ShowTutorialPrompt(FText Message, FKey KeyToWait, TArray<FKey> ExtraAllowedKeys, bool bRequireCombo)
{
	if (PromptText) PromptText->SetText(Message);
    
	RequiredKey = KeyToWait;
	AllowedKeys = ExtraAllowedKeys;
	bIsComboRequired = bRequireCombo; // [추가]

    SetVisibility(ESlateVisibility::Visible);
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.001f);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
       FInputModeGameAndUI InputMode;
       InputMode.SetWidgetToFocus(TakeWidget());
       InputMode.SetHideCursorDuringCapture(true);
       PC->SetInputMode(InputMode);
    }
}

// =========================================================
// [안전 구역] 정답 확인은 무조건 Tick에서 처리합니다.
// =========================================================
void UTutorialWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (RequiredKey.IsValid() && GetVisibility() == ESlateVisibility::Visible)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			// 1. 목표 키(Shift 등)가 눌렸는가?
			if (PC->IsInputKeyDown(RequiredKey))
			{
				bool bComboSatisfied = true;

				// 2. 동시 입력이 필요하다면, 허용된 키(방향키) 중 하나라도 눌려있는지 검사!
				if (bIsComboRequired && AllowedKeys.Num() > 0)
				{
					bComboSatisfied = false;
					for (const FKey& Key : AllowedKeys)
					{
						if (PC->IsInputKeyDown(Key))
						{
							bComboSatisfied = true; // 방향키 하나라도 눌려있으면 합격
							break;
						}
					}
				}

				// 3. 조건이 모두 충족되었을 때만 튜토리얼 해소!
				if (bComboSatisfied)
				{
					CompletePrompt();
				}
			}
		}
	}
}

// =========================================================
// [철벽 방패] 허용되지 않은 키보드/마우스 입력은 여기서 소멸시킵니다.
// =========================================================

FReply UTutorialWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();
    // 정답이거나 허용된 키면 무사 통과 (Unhandled)
    if (Key == RequiredKey || AllowedKeys.Contains(Key)) return FReply::Unhandled();
    
    // 엉뚱한 키면 소멸 (Handled)
    return FReply::Handled();
}

FReply UTutorialWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();
    if (Key == RequiredKey || AllowedKeys.Contains(Key)) return FReply::Unhandled();
    
    return FReply::Handled(); // 뗄 때도 반드시 막아줘야 먹통 버그가 안 생깁니다!
}

FReply UTutorialWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FKey Key = InMouseEvent.GetEffectingButton();
    if (Key == RequiredKey || AllowedKeys.Contains(Key)) return FReply::Unhandled();
    return FReply::Handled();
}

FReply UTutorialWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FKey Key = InMouseEvent.GetEffectingButton();
    if (Key == RequiredKey || AllowedKeys.Contains(Key)) return FReply::Unhandled();
    return FReply::Handled();
}

// =========================================================

void UTutorialWidget::CompletePrompt()
{
    RequiredKey = FKey(); 
    AllowedKeys.Empty(); 

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