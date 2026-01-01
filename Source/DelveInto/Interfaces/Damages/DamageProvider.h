// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DELVEINTO_API IDamageProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float GetBaseDamage() const = 0;
};
