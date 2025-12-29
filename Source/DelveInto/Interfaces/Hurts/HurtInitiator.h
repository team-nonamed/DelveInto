// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Messages/Results/HurtResult.h"
#include "UObject/Interface.h"
#include "HurtInitiator.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHurtInitiator : public UInterface
{
	GENERATED_BODY()
};

/**
 * Hurt를 생성하는 Actor가 구현할 Interface
 * - 신호를 받아 Attack Component로 흐름을 전달
 */
class DELVEINTO_API IHurtInitiator
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// TODO: 어떻게 인식하게 할지에 대해서 고민할 것
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hurt")
	FHurtResult InitHurt();
};
