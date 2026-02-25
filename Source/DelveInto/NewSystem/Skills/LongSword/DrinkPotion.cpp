#include "DrinkPotion.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h"
#include "NewSystem/Entities/Characters/Handlers/HealthHandler.h"
#include "NewSystem/Widgets/HandDisplayWidget.h"
#include "NewSystem/Widgets/HealthBarWidget.h"
#include "NewSystem/Widgets/InventoryWidget.h"

USkill_DrinkPotion::USkill_DrinkPotion()
{
	SkillName = FText::FromString(TEXT("포션 마시기"));
	bHasCooldown = true;
	MaxCooldownSeconds = 3.0f; // 포션 연속 복용 방지 (3초 쿨타임)
	bHasCombo = false;
}

bool USkill_DrinkPotion::ExecuteSkillLogic_Implementation()
{
	ADelveCharacter* Caster = Cast<ADelveCharacter>(GetOwner());
	if (!Caster || !Caster->InventoryHandler || !Caster->HealthHandler) return false;

	if (!PotionItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("포션 스킬에 PotionItemData가 할당되지 않았습니다!"));
		return false;
	}

	bool bHasPotion = Caster->InventoryHandler->ConsumeItem(PotionItemData, 1);

	if (bHasPotion)
	{
		// 1. 실제 체력 데이터 회복
		Caster->HealthHandler->ApplyHeal(HealAmount);
		UE_LOG(LogTemp, Warning, TEXT("포션 꿀꺽! %f 회복. (현재 체력: %f)"), HealAmount, Caster->HealthHandler->CurrentHealth);
        
		// ==========================================================
		// [신규] 2. 체력바 UI 즉시 업데이트!
		// ==========================================================
		if (Caster->WeaponWidgetInstance && Caster->WeaponWidgetInstance->HealthBar)
		{
			// 현재 체력 비율(0.0 ~ 1.0)을 계산해서 체력바 위젯에 전달합니다.
			float HealthRatio = Caster->HealthHandler->CurrentHealth / Caster->HealthHandler->MaxHealth;
			Caster->WeaponWidgetInstance->HealthBar->UpdateHealthRatio(HealthRatio);
		}

		// ==========================================================
		// [신규] 3. 인벤토리 UI 즉시 업데이트! (개수 줄어든 것 반영)
		// ==========================================================
		if (Caster->WeaponWidgetInstance && Caster->WeaponWidgetInstance->Inventory)
		{
			// InitializeSlots 함수를 다시 호출하면 변경된 개수로 인벤토리를 새로고침(리렌더링)합니다.
			Caster->WeaponWidgetInstance->Inventory->InitializeSlots(Caster->InventoryHandler);
		}

		return true; 
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("인벤토리에 포션이 없습니다!"));
		return false;
	}
}