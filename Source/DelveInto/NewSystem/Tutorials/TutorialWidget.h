#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialStepCompleted);

UCLASS()
class DELVEINTO_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnTutorialStepCompleted OnStepCompleted;

	// [수정] 마지막에 bool bRequireCombo = false 를 추가합니다.
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void ShowTutorialPrompt(FText Message, FKey KeyToWait, TArray<FKey> ExtraAllowedKeys, bool bRequireCombo = false);

protected:
	virtual void NativeConstruct() override;
    
	// [추가] 이벤트 도중이 아닌, 안전한 Tick에서 정답을 확인합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// [핵심] 키보드를 누를 때와 뗄 때 모두 방어해야 엔진이 고장나지 않습니다!
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// 마우스도 마찬가지로 누를 때와 뗄 때 모두 방어합니다.
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PromptText;

private:
	FKey RequiredKey;
	TArray<FKey> AllowedKeys;

	void CompletePrompt();

	// [추가] 동시 입력이 필요한지 기억할 변수
	bool bIsComboRequired;
};