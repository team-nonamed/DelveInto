#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Skills/SkillBase.h" // 프로젝트 경로에 맞게 수정하세요
#include "DashSlash.generated.h"

UCLASS(Blueprintable, ClassGroup=(Skill), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkill_DashSlash : public USkillBase
{
	GENERATED_BODY()

public:
	USkill_DashSlash();

protected:
	// USkillBase의 실제 발동 로직 오버라이드
	virtual bool ExecuteSkillLogic_Implementation() override;

	// 돌진하는 힘 (속도)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Dash")
	float DashForce = 2500.0f;

	// 앞으로 베며 나아갈 거리 (판정 길이)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Dash")
	float SlashRange = 400.0f;

	// 크게 베는 범위 (구체의 반지름)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Dash")
	float SlashRadius = 150.0f;
};