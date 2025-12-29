// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Messages/Results/HurtResult.h"
#include "UObject/Interface.h"
#include "HurtHandler.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHurtHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DELVEINTO_API IHurtHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hurt")
	FHurtResult HandleHurt(FHurtRequest& Request);
};
