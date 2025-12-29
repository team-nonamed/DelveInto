// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hurts/HurtReceiver.h"
#include "AbstractEntity.generated.h"

UCLASS(Abstract)
class DELVEINTO_API AAbstractEntity : public AActor, public IHurtReceiver
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAbstractEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
