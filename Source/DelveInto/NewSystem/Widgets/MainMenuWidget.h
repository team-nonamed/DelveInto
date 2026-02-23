#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookWidget.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UTextBlock; // 텍스트 제어용 헤더
class UFlipbookPlayerWidget;
class UPaperFlipbook;

UCLASS()
class DELVEINTO_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// --- 구성 요소 ---
	UPROPERTY(meta = (BindWidget))
	UPaperFlipbookWidget* IntroPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound")
	USoundBase* IntroSound;

	// 투명 버튼 (클릭 영역 담당)
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// 실제 표시되는 텍스트 (색상 변경 담당)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuitText;

	// --- 설정 데이터 ---
	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor NormalColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor HoverColor = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, Category = "Setup")
	UPaperFlipbook* IntroFlipbookAsset;

	UPROPERTY(EditAnywhere, Category = "Setup")
	FName StartLevelName = TEXT("MainLevel");

	// --- 콜백 함수 ---
	UFUNCTION()
	void HandleIntroFinished();

	// 마우스 호버 이벤트
	UFUNCTION() void OnStartHovered();
	UFUNCTION() void OnStartUnhovered();
	UFUNCTION() void OnQuitHovered();
	UFUNCTION() void OnQuitUnhovered();

	UFUNCTION() void OnStartClicked();
	UFUNCTION() void OnQuitClicked();
};