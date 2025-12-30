// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HurtReceiver.h"
#include "Interfaces/Sender.h"
#include "Messages/Results/HurtResult.h"
#include "UObject/Interface.h"
#include "HurtSender.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHurtSender : public USender
{
	GENERATED_BODY()
};

/**
 * Hurt를 다른 Actor에게 전달하는 Component가 구현하는 Interface
 */
class DELVEINTO_API IHurtSender
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hurt")
	FHurtResult SendHurt(AActor* HurtInitiator, const TScriptInterface<IHurtReceiver> Receiver);
	
};
