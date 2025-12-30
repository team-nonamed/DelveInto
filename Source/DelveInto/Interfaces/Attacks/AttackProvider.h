// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttackProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DELVEINTO_API IAttackProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float GetBaseAttack();
};
