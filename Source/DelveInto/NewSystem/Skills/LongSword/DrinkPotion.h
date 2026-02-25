#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Skills/SkillBase.h" // 경로 맞춰주세요!
#include "DrinkPotion.generated.h"

class UItemData;

UCLASS(Blueprintable)
class DELVEINTO_API USkill_DrinkPotion : public USkillBase
{
	GENERATED_BODY()

public:
	USkill_DrinkPotion();

protected:
	virtual bool ExecuteSkillLogic_Implementation() override;

	// 회복할 체력량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Potion")
	float HealAmount = 30.0f;

	// 소모할 포션의 원본 데이터 (상점에서 샀던 그 PotionItemData와 똑같은 걸 넣어야 함)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Potion")
	UItemData* PotionItemData; 
};