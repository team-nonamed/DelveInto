// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Entities/DelveProjectile.h"
#include "NewSystem/Skills/SkillBase.h"
#include "AlterAttack.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Skill), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkill_LongSword_AlterAttack : public USkillBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkill_LongSword_AlterAttack();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Combat")
	TSubclassOf<ADelveProjectile> SwordWaveClass;

protected:
	virtual bool ExecuteSkillLogic_Implementation() override;

	virtual float CalculateDamage() const override;
};
