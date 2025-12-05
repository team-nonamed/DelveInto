// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimatedImageWidget.h"

UAnimatedImageWidget::UAnimatedImageWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UAnimatedImageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentFrameIndex = 0;
	TimeAccumulator = 0.0f;
	ApplyCurrentFrame();

	if (bAutoPlay)
	{
		Play();
	}
}

void UAnimatedImageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bPlaying)
	{
		return;
	}

	const int32 NumFrames = Frames.Num();
	if (NumFrames <= 0 || FrameRate <= 0.f)
	{
		return;
	}

	const float FrameTime = 1.0f / FrameRate;
	TimeAccumulator += InDeltaTime;

	// 여러 프레임을 한 번에 건너뛰는 경우도 대비해서 while 사용
	while (TimeAccumulator >= FrameTime)
	{
		TimeAccumulator -= FrameTime;
		StepFrame();
	}
}

void UAnimatedImageWidget::ApplyCurrentFrame()
{
	if (!Image)
	{
		return;
	}

	if (!Frames.IsValidIndex(CurrentFrameIndex))
	{
		return;
	}

	if (UTexture2D* FrameTex = Frames[CurrentFrameIndex])
	{
		Image->SetBrushFromTexture(FrameTex, true);
	}
}

void UAnimatedImageWidget::StepFrame()
{
	const int32 NumFrames = Frames.Num();
	if (NumFrames <= 0)
	{
		return;
	}

	CurrentFrameIndex++;

	if (CurrentFrameIndex >= NumFrames)
	{
		if (bLoop)
		{
			CurrentFrameIndex = 0;
		}
		else
		{
			// 마지막 프레임에서 멈춤
			CurrentFrameIndex = NumFrames - 1;
			bPlaying = false;
		}
	}

	ApplyCurrentFrame();
}

void UAnimatedImageWidget::Play()
{
	if (Frames.Num() <= 0)
	{
		return;
	}

	bPlaying = true;
}

void UAnimatedImageWidget::Pause()
{
	bPlaying = false;
}

void UAnimatedImageWidget::Stop()
{
	bPlaying = false;
	CurrentFrameIndex = 0;
	TimeAccumulator = 0.0f;
	ApplyCurrentFrame();
}

void UAnimatedImageWidget::SetFrame(int32 NewFrameIndex)
{
	if (!Frames.IsValidIndex(NewFrameIndex))
	{
		return;
	}

	CurrentFrameIndex = NewFrameIndex;
	TimeAccumulator = 0.0f;
	ApplyCurrentFrame();
}