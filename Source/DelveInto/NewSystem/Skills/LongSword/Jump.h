// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Skills/SkillBase.h"
#include "Jump.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkill_LongSword_Jump : public USkillBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkill_LongSword_Jump();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual bool ExecuteSkillLogic_Implementation() override;

private:
	// 캐릭터가 땅에 닿았을 때 호출될 함수
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);
};
