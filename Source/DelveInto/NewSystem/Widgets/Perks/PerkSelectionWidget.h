#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewSystem/Enhances/PerkDBSubsystem.h"
#include "PerkSelectionWidget.generated.h"

class UPerkHandler;
class UPerkChoiceWidget;
class UPaperFlipbook;
class UImage;
class UPerkBase;

UCLASS(Abstract)
class DELVEINTO_API UPerkSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
    
	// [핵심] 타이머를 대체할 틱 함수
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowChoices(UPerkHandler* PlayerPerkHandler, int32 ChoiceCount = 3);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPerkChoiceWidget> ChoiceWidget0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPerkChoiceWidget> ChoiceWidget1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPerkChoiceWidget> ChoiceWidget2;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Animation")
	TObjectPtr<UPaperFlipbook> InitFlipbook;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Animation")
	TObjectPtr<UPaperFlipbook> PostInitFlipbook;

private:
	UPROPERTY()
	UPerkHandler* TargetHandler;

	UPROPERTY()
	TArray<UPerkChoiceWidget*> ChoiceWidgets;
    
	// [수정] 뽑힌 퍽 임시 저장소를 구조체 배열로 변경
	UPROPERTY()
	TArray<FPerkChoiceData> FetchedPerks;

	int32 CachedChoiceCount;

	// --- 재생 제어용 변수들 ---
	bool bIsPlayingInit;
	bool bIsPlayingPostInit;
	bool bIsRevealing;
    
	int32 CurrentInitFrame;
	int32 CurrentPostInitFrame;
	int32 CardsRevealedCount;
    
	float FrameAccumulator;
	float RevealAccumulator;

	// --- 내부 제어 함수 ---
	UFUNCTION()
	void OnPerkChosen(UPerkBase* SelectedPerk, int32 TargetLevel);

	void UpdateInitFrame();
	void StartPostInitFlipbook();
	void UpdatePostInitFrame();
	void RevealChoices();
};