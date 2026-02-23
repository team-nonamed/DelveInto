#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperFlipbookWidget.generated.h"

class UImage;
class UPaperFlipbook;

// 애니메이션 종료 시 호출될 델리게이트 정의
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlipbookFinished);

/**
 * PaperFlipbookWidget이 없을 때 이를 대신하며 종료 이벤트를 제공하는 클래스
 */
UCLASS()
class DELVEINTO_API UPaperFlipbookWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 애니메이션이 끝나면 블루프린트에서 호출될 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Flipbook|Events")
	FOnFlipbookFinished OnFinished;

	// 애니메이션 재생 시작 함수
	UFUNCTION(BlueprintCallable, Category = "Flipbook|Functions")
	void PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop = false);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 이 위젯이 스프라이트를 그릴 Image 컴포넌트 (블루프린트에서 이름 일치 필수)
	UPROPERTY(meta = (BindWidget))
	UImage* TargetImage;

private:
	UPROPERTY()
	UPaperFlipbook* CurrentFlipbook;

	float AccumulatedTime = 0.0f;
	bool bIsPlaying = false;
	bool bIsLooping = false;
};