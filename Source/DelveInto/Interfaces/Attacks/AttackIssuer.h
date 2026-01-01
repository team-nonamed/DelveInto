// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Damages/DamageModifier.h"
#include "Interfaces/Hurts/HurtReceiver.h"
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
	 * @param Instigator 공격자
	 * @param Receiver 피해를 받는 대상
	 * @param Skill 사용한 스킬
	 * @param IsCritical 치명타 여부
	 * @return 공격 결과
	 */
	virtual FHurtResult IssueAttack(TScriptInterface<IAttackInstigator>& Instigator,
		TScriptInterface<IHurtReceiver>& Receiver,
		TScriptInterface<IDamageModifier>& Skill,
		bool IsCritical = false) = 0;
};
