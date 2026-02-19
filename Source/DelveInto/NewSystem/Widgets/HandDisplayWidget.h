#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperFlipbook.h"
#include "Components/Image.h"
#include "HealthBarWidget.h" // 체력바 헤더 확인 필요
#include "HandDisplayWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHandDisplayWidget, Log, All);

UCLASS()
class DELVEINTO_API UHandDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHealthBarWidget> HealthBar;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HandImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	TObjectPtr<UPaperFlipbook> CurrentFlipbook;

	float PlaySpeed = 1.0f;
	float AccumulatedTime = 0.0f;
	bool bIsLooping = false;
	bool bIsPlaying = false;

public:
	// 재생 속도 기본값 1.0f
	UFUNCTION(BlueprintCallable, Category = "Weapon UI")
	void PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop = true, float NewPlaySpeed = 1.0f);

	// 강제 정지 (아이템 해제 등)
	UFUNCTION(BlueprintCallable, Category = "Weapon UI")
	void Stop();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
	// 내부적으로 이미지 리소스만 갈아끼우는 함수
	void UpdateImageResource(int32 FrameIndex);
};