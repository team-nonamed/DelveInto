#pragma once

#include "CoreMinimal.h"
#include "DelveHealthBarWidget.h"
#include "Blueprint/UserWidget.h"
#include "PaperFlipbook.h"
#include "Components/Image.h"
#include "WeaponDisplayWidget.generated.h"

UCLASS()
class DELVEINTO_API UWeaponDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부(캐릭터/무기)에서 재생할 플립북을 넣어주는 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon UI")
	void PlayFlipbook(UPaperFlipbook* NewFlipbook, bool bLoop = true);

	// UMG 에디터에 배치할 이미지 위젯 (이름을 반드시 "WeaponImage"로 맞춰야 함)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDelveHealthBarWidget> HealthBar;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// UMG 에디터에 배치할 이미지 위젯 (이름을 반드시 "WeaponImage"로 맞춰야 함)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;

	

private:
	TObjectPtr<UPaperFlipbook> CurrentFlipbook;
    
	float AccumulatedTime = 0.0f;
	bool bIsLooping = false;
	bool bIsPlaying = false;

	// 스프라이트를 UMG 브러시로 변환하는 헬퍼
	void UpdateBrush(UPaperSprite* Sprite);
};