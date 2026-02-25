#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperFlipbookWidget.generated.h"

class UImage;
class UPaperFlipbook;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlipbookFinished);

UCLASS()
class DELVEINTO_API UPaperFlipbookWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Flipbook|Events")
	FOnFlipbookFinished OnFinished;

	UFUNCTION(BlueprintCallable, Category = "Flipbook|Functions")
	void PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop = false);

	// [신규] 애니메이션 강제 정지 및 잔상 제거
	UFUNCTION(BlueprintCallable, Category = "Flipbook|Functions")
	void StopFlipbook();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UImage* TargetImage;

private:
	UPROPERTY()
	UPaperFlipbook* CurrentFlipbook;

	float AccumulatedTime = 0.0f;
	bool bIsPlaying = false;
	bool bIsLooping = false;
};