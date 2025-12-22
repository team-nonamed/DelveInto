// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/Messages/Requests/HurtRequest.h"
#include "UObject/Object.h"
#include "HurtReceiver.generated.h"

/**
 * 피해를 입을 수 있는 Actor들이 구현하는 Interface
 */
UINTERFACE(BlueprintType)
class UHurtReceiver : public UInterface
{
	GENERATED_BODY()
};

class DELVEINTO_API IHurtReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damageable")
	void ReceiveHurt(FHurtRequest& Request);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Meta Data")
	const FString GetName() const;
};