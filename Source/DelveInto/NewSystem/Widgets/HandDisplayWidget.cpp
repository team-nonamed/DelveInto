#include "HandDisplayWidget.h"
#include "PaperSprite.h"
#include "Components/TextBlock.h"
#include "NewSystem/Widgets/HealthBarWidget.h"
#include "NewSystem/Widgets/InventoryWidget.h"
#include "NewSystem/Widgets/PaperFlipbookWidget.h"
#include "NewSystem/Widgets/SkillBarWidget.h"
#include "NewSystem/Entities/Characters/Handlers/CombatHandler.h"

DEFINE_LOG_CATEGORY(LogHandDisplayWidget);

void UHandDisplayWidget::InitializeUI(UInventoryHandler* InHandler, UCombatHandler* InCombatHandler)
{
    if (Inventory) Inventory->InitializeSlots(InHandler);
    if (SkillWidget) SkillWidget->InitializeSkillBar(InCombatHandler);
    
    if (HitEffectWidget)
    {
        HitEffectWidget->StopFlipbook();
    }

	if (InHandler)
	{
		UpdateGoldDisplay(InHandler->GoldAmount);
	}
}

void UHandDisplayWidget::UpdateGoldDisplay(int32 CurrentGold)
{
	if (GoldText)
	{
		GoldText->SetText(FText::AsNumber(CurrentGold));
	}
}

void UHandDisplayWidget::PlayHitEffect(UPaperFlipbook* HitFlipbook)
{
    if (HitEffectWidget && HitFlipbook)
    {
        HitEffectWidget->PlayFlipbook(HitFlipbook, false);
    }
}

void UHandDisplayWidget::PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop, float NewPlaySpeed)
{
    if (CurrentFlipbook != NewFlipbook || !bIsPlaying)
    {
       AccumulatedTime = 0.0f;
    }

    CurrentFlipbook = NewFlipbook;
    this->PlaySpeed = FMath::Max(0.01f, NewPlaySpeed);
    bIsLooping = bLoop;
    bIsPlaying = true;

    if (CurrentFlipbook)
    {
       UpdateImageResource(0);
       if (HandImage) HandImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    } 
}

void UHandDisplayWidget::Stop()
{
    bIsPlaying = false;
    if (HandImage) HandImage->SetBrushResourceObject(nullptr);
}

void UHandDisplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!HandImage || !CurrentFlipbook || !bIsPlaying) return;

    AccumulatedTime += (InDeltaTime * PlaySpeed);

    int32 FrameIndex = 0;
    float TotalDuration = CurrentFlipbook->GetTotalDuration();

    if (TotalDuration > 0.0f)
    {
       if (bIsLooping)
       {
          float Pos = FMath::Fmod(AccumulatedTime, TotalDuration);
          FrameIndex = CurrentFlipbook->GetKeyFrameIndexAtTime(Pos);
       }
       else
       {
          if (AccumulatedTime >= TotalDuration)
          {
             FrameIndex = CurrentFlipbook->GetNumKeyFrames() - 1;
          }
          else
          {
             FrameIndex = CurrentFlipbook->GetKeyFrameIndexAtTime(AccumulatedTime);
          }
       }
    }

    UpdateImageResource(FrameIndex);
}

void UHandDisplayWidget::UpdateImageResource(int32 FrameIndex)
{
    if (!CurrentFlipbook || !HandImage) return;
    
    UPaperSprite* Sprite = CurrentFlipbook->GetSpriteAtFrame(FrameIndex);
    HandImage->SetBrushResourceObject(Sprite ? Sprite : nullptr);
}