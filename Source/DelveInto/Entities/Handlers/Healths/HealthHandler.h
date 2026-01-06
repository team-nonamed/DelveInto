// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/Handlers/AbstractHealthHandler.h"
#include "HealthHandler.generated.h"


UCLASS(BlueprintType, ClassGroup=("Attack"), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UHealthHandler : public UAbstractHealthHandler
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
};
