#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewSystem/Skills/SkillBase.h"
#include "SkillSlotWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class DELVEINTO_API USkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownText;

	// 런타임에 머티리얼 파라미터를 조절하기 위한 인스턴스
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CooldownDynamicMaterial;

	UPROPERTY()
	TWeakObjectPtr<USkillBase> TargetSkill;

public:
	// 슬롯에 추적할 스킬을 할당합니다.
	void BindSkill(USkillBase* InSkill);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};