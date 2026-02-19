// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Skills/SkillBase.h"
#include "BaseAttack.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Skill), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkill_LongSword_BaseAttack : public USkillBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkill_LongSword_BaseAttack();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(ClampMin = "0.0"))
	float SecondComboDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(ClampMin = "0.0"))
	float ThirdComboDamage = 15.0f;

protected:
	// 데미지 계산 (콤보 3타째 추가 데미지 등)
	virtual float CalculateDamage() const override;

	// 실제 판정 및 사운드 재생 로직
	virtual bool ExecuteSkillLogic_Implementation() override;
};
