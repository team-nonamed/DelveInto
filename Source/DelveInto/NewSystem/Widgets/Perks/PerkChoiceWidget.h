#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PerkChoiceWidget.generated.h"

class UPerkBase;
class UButton;
class UTextBlock;
class UImage;
class UWidgetAnimation; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerkSelectedSignature, UPerkBase*, SelectedPerk, int32, TargetLevel);

UCLASS(Abstract)
class DELVEINTO_API UPerkChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// [수정] 부모 위젯에서 정확한 타이밍에 호출하므로 Delay 파라미터는 삭제합니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetupChoice(UPerkBase* InPerk, int32 InNextLevel);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPerkSelectedSignature OnPerkSelected;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	// [유지] 퍽의 기본 설명
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DescriptionText;

	// [신규 유지] 퍽의 레벨별 추가 설명
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelDescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeIn;

private:
	UPROPERTY()
	UPerkBase* CachedPerk;

	int32 CachedLevel;

	UFUNCTION()
	void HandleButtonClicked();
};