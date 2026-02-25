#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewSystem/Weapons/WeaponSkillSlot.h" // 프로젝트의 Enum 경로에 맞게 수정
#include "SkillBarWidget.generated.h"

class USkillSlotWidget;
class UCombatHandler;
class USkillBase;

UCLASS()
class DELVEINTO_API USkillBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI|Skill")
	void InitializeSkillBar(UCombatHandler* InCombatHandler);

protected:
	virtual void NativeConstruct() override;

	// [수정] 파라미터 제거
	UFUNCTION()
	void OnWeaponSkillsUpdated();

	// UI 블루프린트에서 매칭할 개별 슬롯들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillSlotWidget> PrimarySlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillSlotWidget> SecondarySlot;

protected:
	// [신규] 데이터를 가져오기 위해 CombatHandler를 약참조로 기억해 둡니다.
	UPROPERTY()
	TWeakObjectPtr<UCombatHandler> CachedCombatHandler;

	// 필요에 따라 슬롯을 추가하세요 (BindWidgetOptional을 쓰면 UI에 없어도 크래시가 나지 않습니다)
	// UPROPERTY(meta = (BindWidgetOptional))
	// TObjectPtr<USkillSlotWidget> UltimateSlot;

	// 런타임에 슬롯들을 쉽게 관리하고 찾기 위한 내부 맵
	UPROPERTY()
	TMap<EWeaponSkillSlot, USkillSlotWidget*> SlotMap;
};