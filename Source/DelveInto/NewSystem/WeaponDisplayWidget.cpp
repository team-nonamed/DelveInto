#include "WeaponDisplayWidget.h"
#include "PaperSprite.h" // 스프라이트 -> 브러시 변환용
#include "PaperSpriteBlueprintLibrary.h"

void UWeaponDisplayWidget::PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop)
{
	if (CurrentFlipbook != NewFlipbook)
	{
		CurrentFlipbook = NewFlipbook;
		AccumulatedTime = 0.0f;
		bIsPlaying = true;
		bIsLooping = bLoop;
	}
}

void UWeaponDisplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsPlaying || !CurrentFlipbook || !WeaponImage) return;

	// 1. 시간 누적
	AccumulatedTime += InDeltaTime;

	// 2. 현재 시간에 맞는 프레임 인덱스 계산
	int32 FrameIndex = 0;
    
	float TotalDuration = CurrentFlipbook->GetTotalDuration();
	if (TotalDuration > 0.0f)
	{
		// 루프 처리
		if (bIsLooping)
		{
			float PlaybackPos = FMath::Fmod(AccumulatedTime, TotalDuration);
			FrameIndex = CurrentFlipbook->GetKeyFrameIndexAtTime(PlaybackPos);
		}
		else
		{
			// 루프 아닐 때 (공격 모션 등) 끝나면 멈춤
			if (AccumulatedTime >= TotalDuration)
			{
				FrameIndex = CurrentFlipbook->GetNumKeyFrames() - 1; // 마지막 프레임 유지
				// 혹은 여기서 bIsPlaying = false 처리 후 Idle로 복귀 요청 가능
			}
			else
			{
				FrameIndex = CurrentFlipbook->GetKeyFrameIndexAtTime(AccumulatedTime);
			}
		}
	}

	// 3. 해당 프레임의 스프라이트 가져오기
	UPaperSprite* Sprite = CurrentFlipbook->GetSpriteAtFrame(FrameIndex);

	// 4. 이미지 위젯 업데이트
	UpdateBrush(Sprite);
}

void UWeaponDisplayWidget::UpdateBrush(UPaperSprite* Sprite)
{
	if (!Sprite || !WeaponImage) return;

	// [수정됨] 라이브러리 없이 직접 FSlateBrush 생성 (안전한 방법)
	FSlateBrush NewBrush;
	NewBrush.SetResourceObject(Sprite);
    
	// 원본 비율 유지를 원하면 아래 주석 해제 (Image 위젯 설정을 따르는 게 보통 더 편함)
	// if (Sprite->GetSourceSize().X > 0 && Sprite->GetSourceSize().Y > 0)
	// {
	//    NewBrush.ImageSize = FVector2D(Sprite->GetSourceSize().X, Sprite->GetSourceSize().Y);
	// }

	WeaponImage->SetBrush(NewBrush);
}