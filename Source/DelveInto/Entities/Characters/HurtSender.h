// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HurtReceiver.h"
#include "UObject/Object.h"
#include "HurtSender.generated.h"

/**
 * 피해를 입을 수 있는 Actor들이 구현하는 Interface
 */
UINTERFACE(BlueprintType)
class UHurtSender : public UInterface
{
	GENERATED_BODY()
};

class DELVEINTO_API IHurtSender
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hurt | Send")
	void SendHurt(AActor* Receiver);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hurt | Send")
	const FString GetName() const;

protected:
	
};