
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

	// 첫 프레임 즉시 적용
	if (UPaperSprite* StartSprite = CurrentFlipbook->GetSpriteAtTime(0.0f))
	{
		// [수정] C++에서는 SetBrushResourceObject를 사용합니다.
		TargetImage->SetBrushResourceObject(StartSprite);
	}
}

void UPaperFlipbookWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsPlaying && CurrentFlipbook && TargetImage)
	{
		AccumulatedTime += InDeltaTime;

		// 현재 시간에 맞는 스프라이트 가져오기
		UPaperSprite* CurrentSprite = CurrentFlipbook->GetSpriteAtTime(AccumulatedTime);
		if (CurrentSprite)
		{
			// [수정] C++에서는 SetBrushResourceObject를 사용합니다.
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
				bIsPlaying = false;
				if (OnFinished.IsBound())
				{
					OnFinished.Broadcast();
				}
			}
		}
	}
}