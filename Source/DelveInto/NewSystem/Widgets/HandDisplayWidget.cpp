#include "HandDisplayWidget.h"
#include "PaperSprite.h"

DEFINE_LOG_CATEGORY(LogHandDisplayWidget);

void UHandDisplayWidget::PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop, float NewPlaySpeed)
{
    // 플립북이 바뀌거나, 멈춰있었다면 시간 초기화
    if (CurrentFlipbook != NewFlipbook || !bIsPlaying)
    {
       AccumulatedTime = 0.0f;
    }

    CurrentFlipbook = NewFlipbook;
    
    // [버그 수정] 인자로 들어온 NewPlaySpeed를 멤버 변수에 저장해야 함
    this->PlaySpeed = FMath::Max(0.01f, NewPlaySpeed);
    
    bIsLooping = bLoop;
    bIsPlaying = true;

    // 첫 프레임 즉시 갱신
    if (CurrentFlipbook)
    {
       UpdateImageResource(0);
       
       // 만약 HandImage가 숨겨져 있었다면 보이게 설정 (옵션)
       HandImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    } 
}

void UHandDisplayWidget::Stop()
{
    bIsPlaying = false;
    if (HandImage)
    {
        // 이미지를 투명하게 하거나 숨김
        HandImage->SetBrushResourceObject(nullptr);
    }
}

void UHandDisplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!HandImage || !CurrentFlipbook || !bIsPlaying) return;

    // 1. 시간 누적
    AccumulatedTime += (InDeltaTime * PlaySpeed);

    // 2. 프레임 인덱스 계산
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
          // 루프가 아닌 경우: 끝나면 마지막 프레임 유지
          if (AccumulatedTime >= TotalDuration)
          {
             FrameIndex = CurrentFlipbook->GetNumKeyFrames() - 1;
             // 필요하다면 여기서 bIsPlaying = false; 처리 가능
          }
          else
          {
             FrameIndex = CurrentFlipbook->GetKeyFrameIndexAtTime(AccumulatedTime);
          }
       }
    }

    // 3. 이미지 업데이트
    UpdateImageResource(FrameIndex);
}

void UHandDisplayWidget::UpdateImageResource(int32 FrameIndex)
{
    if (!CurrentFlipbook || !HandImage) return;
    
    UPaperSprite* Sprite = CurrentFlipbook->GetSpriteAtFrame(FrameIndex);
    
    // [최적화] FSlateBrush를 새로 만드는 것보다 ResourceObject만 바꾸는 게 훨씬 가벼움
    if (Sprite)
    {
        HandImage->SetBrushResourceObject(Sprite);
    }
    else
    {
        // 스프라이트가 없는 프레임(공백)일 수도 있으므로 null 처리
        HandImage->SetBrushResourceObject(nullptr);
    }
}