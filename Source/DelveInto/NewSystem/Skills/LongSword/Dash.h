#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Skills/SkillBase.h"
#include "Dash.generated.h"

/**
 * 이동 방향 또는 바라보는 방향으로 빠르게 회피/이동하는 대시 스킬
 */
UCLASS(Abstract, Blueprintable, ClassGroup=(Skill), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkill_LongSword_Dash : public USkillBase
{
	GENERATED_BODY()

public:
	USkill_LongSword_Dash();

protected:
	virtual bool ExecuteSkillLogic_Implementation() override;

public:
	// 대시의 강도 (얼마나 멀리, 빠르게 이동할 것인가)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Dash")
	float DashStrength = 2500.0f;

	// 공중으로 대시(Z축 이동)를 허용할 것인가? (보통 땅에서만 구르려면 false)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Dash")
	bool bAllowVerticalDash = false;
};