#include "PaperFlipbookWidget.h"
#include "Components/Image.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"

void UPaperFlipbookWidget::PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop)
{
	if (!NewFlipbook || !TargetImage) return;

	CurrentFlipbook = NewFlipbook;
	bIsLooping = bLoop;
	AccumulatedTime = 0.0f;
	bIsPlaying = true;
    
	// 이펙트 재생 시작 시 보이게 처리
	TargetImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (UPaperSprite* StartSprite = CurrentFlipbook->GetSpriteAtTime(0.0f))
	{
		TargetImage->SetBrushResourceObject(StartSprite);
	}
}

void UPaperFlipbookWidget::StopFlipbook()
{
	bIsPlaying = false;
	if (TargetImage)
	{
		TargetImage->SetBrushResourceObject(nullptr);
		// 이펙트 종료 시 화면을 가리지 않게 숨김 처리
		TargetImage->SetVisibility(ESlateVisibility::Collapsed); 
	}
}

void UPaperFlipbookWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsPlaying && CurrentFlipbook && TargetImage)
	{
		AccumulatedTime += InDeltaTime;

		UPaperSprite* CurrentSprite = CurrentFlipbook->GetSpriteAtTime(AccumulatedTime);
		if (CurrentSprite)
		{
			TargetImage->SetBrushResourceObject(CurrentSprite);
		}

		// 애니메이션 종료 체크
		if (AccumulatedTime >= CurrentFlipbook->GetTotalDuration())
		{
			if (bIsLooping)
			{
				AccumulatedTime = 0.0f;
			}
			else
			{
				// [수정] 재생이 끝나면 스스로 지우고 이벤트 방송
				StopFlipbook();
				if (OnFinished.IsBound())
				{
					OnFinished.Broadcast();
				}
			}
		}
	}
}