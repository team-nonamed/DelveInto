// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Damages/DamageModifier.h"
#include "Interfaces/Hurts/HurtReceiver.h"
#include "Types/SkillDesignator.h"
#include "UObject/Interface.h"
#include "AttackIssuer.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAttackIssuer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DELVEINTO_API IAttackIssuer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Hurt와 관련한 정보를 처리해 공격자 측의 피해량을 계산한 후
	 * Receiver에게 인계하는 Method
	 * @param Designator 선택한 Skill의 지정자
	 */
	virtual FHurtResult IssueAttack(
		ESkillDesignator Designator
		);
};
