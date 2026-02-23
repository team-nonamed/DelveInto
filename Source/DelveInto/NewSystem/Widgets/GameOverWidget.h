#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UButton;

UCLASS()
class DELVEINTO_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	// Retry 버튼이 눌렸을 때 실행될 함수
	UFUNCTION()
	void OnRetryClicked();

	// 블루프린트에서 만들 버튼과 변수명을 반드시 똑같이 맞춰야 연결됩니다.
	UPROPERTY(meta = (BindWidget))
	UButton* RetryButton;
};