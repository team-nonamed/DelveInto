// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Messages/Results/HurtResult.h"
#include "UObject/Interface.h"
#include "AttackInstigator.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UAttackInstigator : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DELVEINTO_API IAttackInstigator
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FHurtResult InstigateAttack(ESkillDesignator Designator) = 0;
};
