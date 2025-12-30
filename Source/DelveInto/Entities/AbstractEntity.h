// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Handlers/AbstractHealthHandler.h"
#include "Interfaces/Hurts/HurtReceiver.h"
#include "AbstractEntity.generated.h"

UCLASS(Abstract)
class DELVEINTO_API AAbstractEntity : public AActor, public IHurtReceiver
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TSubclassOf<UAbstractHealthHandler> HealthHandlerClass;

	UPROPERTY(Transient)
	TObjectPtr<UAbstractHealthHandler> HealthHandler;
	
	// Sets default values for this actor's properties
	AAbstractEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Components|Health")
	virtual FHurtResult ReceiveHurt(const FHurtRequest& Request) override;
};
