#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"

class UTextBlock;

// 올바른 키를 눌렀을 때 매니저에게 알리는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialStepCompleted);

UCLASS()
class DELVEINTO_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnTutorialStepCompleted OnStepCompleted;

	// 매니저가 이 함수를 호출해 튜토리얼 창을 띄웁니다.
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void ShowTutorialPrompt(FText Message, FKey KeyToWait);

protected:
	virtual void NativeConstruct() override;

	// [추가] 매 프레임 키 입력을 감시할 함수
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PromptText;

private:
	FKey RequiredKey;
	void CompletePrompt();
};