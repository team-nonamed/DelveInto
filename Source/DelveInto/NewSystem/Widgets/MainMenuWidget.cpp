#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
    // 중복 호출 제거, 한 번만 호출합니다.
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("MainMenu Widget Constructed!"));

    // 사운드 재생 로직
    if (IntroSound)
    {
        PlaySound(IntroSound);
    }

    // 1. 초기 상태: 버튼 전체(텍스트 포함) 숨기기
    if (StartButton) StartButton->SetVisibility(ESlateVisibility::Collapsed);
    if (QuitButton) QuitButton->SetVisibility(ESlateVisibility::Collapsed);

    // 2. 이벤트 연결 (클릭 + 호버) 
    // [중요] 중복 실행 크래시를 막기 위해 모두 AddUniqueDynamic으로 변경했습니다.
    if (StartButton)
    {
        StartButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnStartClicked);
        StartButton->OnHovered.AddUniqueDynamic(this, &UMainMenuWidget::OnStartHovered);
        StartButton->OnUnhovered.AddUniqueDynamic(this, &UMainMenuWidget::OnStartUnhovered);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnQuitClicked);
        QuitButton->OnHovered.AddUniqueDynamic(this, &UMainMenuWidget::OnQuitHovered);
        QuitButton->OnUnhovered.AddUniqueDynamic(this, &UMainMenuWidget::OnQuitUnhovered);
    }

    
    
    // 3. 인트로 플레이어 설정 및 사운드 재생
    // 흩어져 있던 두 개의 IntroPlayer 블록을 하나로 합쳤습니다.
    if (IntroPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("IntroPlayer Found!")); 
        
        // 인트로 종료 이벤트 연결
        IntroPlayer->OnFinished.AddUniqueDynamic(this, &UMainMenuWidget::HandleIntroFinished);
        
        if (IntroFlipbookAsset)
        {
            
            
            IntroPlayer->PlayFlipbook(IntroFlipbookAsset, false);
            
            
        }
        else 
        {
            UE_LOG(LogTemp, Error, TEXT("IntroFlipbookAsset is NULL!"));
            HandleIntroFinished(); 
        }
    }
    else
    {
        HandleIntroFinished();
    }
}

// --- 호버 연출 (색상 변경) ---
void UMainMenuWidget::OnStartHovered() { if (StartText) StartText->SetColorAndOpacity(FSlateColor(HoverColor)); }
void UMainMenuWidget::OnStartUnhovered() { if (StartText) StartText->SetColorAndOpacity(FSlateColor(NormalColor)); }
void UMainMenuWidget::OnQuitHovered() { if (QuitText) QuitText->SetColorAndOpacity(FSlateColor(HoverColor)); }
void UMainMenuWidget::OnQuitUnhovered() { if (QuitText) QuitText->SetColorAndOpacity(FSlateColor(NormalColor)); }

void UMainMenuWidget::HandleIntroFinished()
{
    // 인트로가 끝나면 버튼 영역(텍스트 포함)을 보여줌
    if (StartButton) StartButton->SetVisibility(ESlateVisibility::Visible);
    if (QuitButton) QuitButton->SetVisibility(ESlateVisibility::Visible);
    
    UE_LOG(LogTemp, Display, TEXT("Intro Finished! Text Buttons are now interactive."));
}

void UMainMenuWidget::OnStartClicked()
{
    UGameplayStatics::OpenLevel(this, StartLevelName);
}

void UMainMenuWidget::OnQuitClicked()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
    }
}