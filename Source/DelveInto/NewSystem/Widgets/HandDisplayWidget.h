#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperFlipbook.h"
#include "Components/Image.h"
#include "HandDisplayWidget.generated.h"

class UHealthBarWidget; 
class UInventoryWidget;
class UPaperFlipbookWidget;
class USkillBarWidget;
class UInventoryHandler;
class UCombatHandler;

DECLARE_LOG_CATEGORY_EXTERN(LogHandDisplayWidget, Log, All);

UCLASS()
class DELVEINTO_API UHandDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HandDisplayWidget | UI")
	void InitializeUI(UInventoryHandler* InHandler, UCombatHandler* InCombatHandler);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GoldText;

	UFUNCTION(BlueprintCallable, Category = "HandDisplayWidget | UI")
	void UpdateGoldDisplay(int32 CurrentGold);
    
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHealthBarWidget> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> Inventory;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPaperFlipbookWidget> HitEffectWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillBarWidget> SkillWidget;

	UFUNCTION(BlueprintCallable, Category = "Weapon UI")
	void PlayHitEffect(UPaperFlipbook* HitFlipbook);

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
	UFUNCTION(BlueprintCallable, Category = "Weapon UI")
	void PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop = true, float NewPlaySpeed = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Weapon UI")
	void Stop();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void UpdateImageResource(int32 FrameIndex);
};