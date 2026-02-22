#include "PerkSelectionWidget.h"
#include "PerkChoiceWidget.h"
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"
#include "NewSystem/Enhances/PerkBase.h"
#include "NewSystem/Enhances/PerkDBSubsystem.h" // 경로 확인 요망
#include "Components/Image.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"

void UPerkSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ChoiceWidgets = { ChoiceWidget0, ChoiceWidget1, ChoiceWidget2 };

    for (UPerkChoiceWidget* Widget : ChoiceWidgets)
    {
        if (Widget)
        {
            Widget->SetVisibility(ESlateVisibility::Hidden);
            if (!Widget->OnPerkSelected.IsAlreadyBound(this, &UPerkSelectionWidget::OnPerkChosen))
            {
                Widget->OnPerkSelected.AddDynamic(this, &UPerkSelectionWidget::OnPerkChosen);
            }
        }
    }
    
    // 초기화
    bIsPlayingInit = false;
    bIsPlayingPostInit = false;
    bIsRevealing = false;
    FrameAccumulator = 0.0f;
    RevealAccumulator = 0.0f;

    // [중요] 일시정지 중에도 이 위젯이 틱을 받을 수 있도록 설정합니다.
    // bTickEvenWhenPaused = true;
}

void UPerkSelectionWidget::ShowChoices(UPerkHandler* PlayerPerkHandler, int32 ChoiceCount)
{
    if (!PlayerPerkHandler) return;

    TargetHandler = PlayerPerkHandler;
    CachedChoiceCount = ChoiceCount;

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->SetPause(true);
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly());
    }

    if (InitFlipbook && BackgroundImage)
    {
        CurrentInitFrame = 0;
        FrameAccumulator = 0.0f;
        bIsPlayingInit = true; // 틱에서 재생 시작
        UpdateInitFrame(); 
    }
    else
    {
        StartPostInitFlipbook();
        RevealChoices();
    }
}

// =========================================================================
// UI 자체 틱(Tick): 일시정지 중에도 플립북 타이밍과 카드 등장 타이밍을 계산합니다.
// =========================================================================
void UPerkSelectionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 1. Init 플립북 재생
    if (bIsPlayingInit && InitFlipbook)
    {
        float FrameDuration = 1.0f / (InitFlipbook->GetFramesPerSecond() > 0.0f ? InitFlipbook->GetFramesPerSecond() : 10.0f);
        FrameAccumulator += InDeltaTime;

        if (FrameAccumulator >= FrameDuration)
        {
            FrameAccumulator -= FrameDuration;
            UpdateInitFrame();
        }
    }
    // 2. PostInit 플립북 무한 루프 재생
    else if (bIsPlayingPostInit && PostInitFlipbook)
    {
        float FrameDuration = 1.0f / (PostInitFlipbook->GetFramesPerSecond() > 0.0f ? PostInitFlipbook->GetFramesPerSecond() : 10.0f);
        FrameAccumulator += InDeltaTime;

        if (FrameAccumulator >= FrameDuration)
        {
            FrameAccumulator -= FrameDuration;
            UpdatePostInitFrame();
        }
    }

    // 3. 카드 순차적 공개 로직 (0.15초 간격)
    if (bIsRevealing)
    {
        RevealAccumulator += InDeltaTime;
        
        float TargetTime = CardsRevealedCount * 0.15f;
        if (RevealAccumulator >= TargetTime)
        {
            if (CardsRevealedCount < ChoiceWidgets.Num())
            {
                UPerkChoiceWidget* Widget = ChoiceWidgets[CardsRevealedCount];
                if (Widget)
                {
                    // 뽑힌 퍽이 있고 유효한지 확인
                    if (CardsRevealedCount < FetchedPerks.Num() && FetchedPerks[CardsRevealedCount].Perk != nullptr)
                    {
                        // [핵심] DB가 미리 계산해서 넘겨준 데이터를 그대로 꺼내 씁니다!
                        FPerkChoiceData ChoiceData = FetchedPerks[CardsRevealedCount];

                        Widget->SetVisibility(ESlateVisibility::Visible);
                        Widget->SetupChoice(ChoiceData.Perk, ChoiceData.NextLevel);
                    }
                    else
                    {
                        Widget->SetVisibility(ESlateVisibility::Collapsed);
                    }
                }
            }
            
            CardsRevealedCount++;
            
            if (CardsRevealedCount >= ChoiceWidgets.Num()) bIsRevealing = false;
        }
    }
}

void UPerkSelectionWidget::UpdateInitFrame()
{
    if (!InitFlipbook || !BackgroundImage)
    {
        bIsPlayingInit = false;
        StartPostInitFlipbook();
        RevealChoices();
        return;
    }

    int32 TotalFrames = InitFlipbook->GetNumFrames();
    
    // Init 애니메이션 종료
    if (CurrentInitFrame >= TotalFrames)
    {
        bIsPlayingInit = false;
        StartPostInitFlipbook();
        RevealChoices();
        return;
    }

    UPaperSprite* Sprite = InitFlipbook->GetSpriteAtFrame(CurrentInitFrame);
    if (Sprite) BackgroundImage->SetBrushResourceObject(Sprite);
    
    CurrentInitFrame++;
}

void UPerkSelectionWidget::StartPostInitFlipbook()
{
    if (!PostInitFlipbook || !BackgroundImage) return;

    CurrentPostInitFrame = 0;
    FrameAccumulator = 0.0f;
    bIsPlayingPostInit = true; 
    UpdatePostInitFrame(); 
}

void UPerkSelectionWidget::UpdatePostInitFrame()
{
    if (!PostInitFlipbook || !BackgroundImage) return;

    int32 TotalFrames = PostInitFlipbook->GetNumFrames();
    if (TotalFrames == 0) return;

    if (CurrentPostInitFrame >= TotalFrames)
    {
        CurrentPostInitFrame = 0; // 루프
    }

    UPaperSprite* Sprite = PostInitFlipbook->GetSpriteAtFrame(CurrentPostInitFrame);
    if (Sprite) BackgroundImage->SetBrushResourceObject(Sprite);
    
    CurrentPostInitFrame++;
}

void UPerkSelectionWidget::RevealChoices()
{
    UPerkDBSubsystem* PerkDB = GetGameInstance()->GetSubsystem<UPerkDBSubsystem>();
    if (!PerkDB) return;

    // DB에서 랜덤 퍽 뽑기
    FetchedPerks = PerkDB->GetRandomPerksForLevelUp(CachedChoiceCount, TargetHandler);

    // 카드 순차 공개 타이머 플래그 On
    bIsRevealing = true;
    CardsRevealedCount = 0;
    RevealAccumulator = 0.0f;
}

void UPerkSelectionWidget::OnPerkChosen(UPerkBase* SelectedPerk, int32 TargetLevel)
{
    if (TargetHandler && SelectedPerk)
    {
        TargetHandler->AddPerk(SelectedPerk, TargetLevel);
    }

    // 타이머 및 플립북 중지
    bIsPlayingInit = false;
    bIsPlayingPostInit = false;
    bIsRevealing = false;

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->SetPause(false);
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    RemoveFromParent();
}