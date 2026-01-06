// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Hurts/HurtHandler.h"
#include "Interfaces/Damages/DamageModifier.h"
#include "AbstractHealthHandler.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHurt, const FHurtResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, const FHurtResult&, LastResult);

UCLASS(Abstract, ClassGroup=(Attack), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UAbstractHealthHandler : public UActorComponent, public IHurtHandler
{
	GENERATED_BODY()

public:
	
	
	// Sets default values for this component's properties
	UAbstractHealthHandler();

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float		MaxHealth;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float		CurrentHealth;

	UPROPERTY(BlueprintReadOnly)
	bool		IsDead;

	UPROPERTY(BlueprintAssignable)
	FOnHurt		OnHurt;

	UPROPERTY(BlueprintAssignable)
	FOnDeath	OnDeath;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<TScriptInterface<IDamageModifier>> Modifiers;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	virtual FHurtResult HandleHurt(const FHurtRequest& Request) override;
};
