// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "AnimatedImageWidget.generated.h"

/**
 * 여러 장의 Texture2D를 일정 FPS로 순환해서 보여주는 UMG 위젯
 */
UCLASS(Abstract, Blueprintable)
class DELVEINTO_API UAnimatedImageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UAnimatedImageWidget(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
		UImage* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
		TArray<TObjectPtr<UTexture2D>> Frames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(ClampMin="1.0"))
	float FrameRate = 12.0f;

	/** 위젯 생성 시 자동 재생할지 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	bool bAutoPlay = true;

	/** 끝까지 재생 후 처음으로 돌아가서 계속 돌릴지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	bool bLoop = true;

	/** 현재 재생 중인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
	bool bPlaying = false;

	/** 현재 프레임 인덱스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
	int32 CurrentFrameIndex = 0;

	/** 내부 시간 누적용 */
	float TimeAccumulator = 0.0f;
	
protected:
	// UUserWidget
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** 현재 CurrentFrameIndex에 해당하는 텍스처를 Image에 적용 */
	void ApplyCurrentFrame();

	/** 프레임 1칸 전진 (루프 처리 포함) */
	void StepFrame();

public:
	/** 재생 시작 (혹은 재개) */
	UFUNCTION(BlueprintCallable, Category="Animation")
	void Play();

	/** 일시정지 (현재 프레임에서 멈춤) */
	UFUNCTION(BlueprintCallable, Category="Animation")
	void Pause();

	/** 정지 후 0번째 프레임으로 리셋 */
	UFUNCTION(BlueprintCallable, Category="Animation")
	void Stop();

	/** 특정 프레임으로 점프 */
	UFUNCTION(BlueprintCallable, Category="Animation")
	void SetFrame(int32 NewFrameIndex);
};
