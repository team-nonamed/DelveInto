// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Attacks/AttackIssuer.h"
#include "Interfaces/Damages/DamageModifier.h"
#include "Interfaces/Damages/DamageProvider.h"
#include "AbstractAttackHandler.generated.h"


UCLASS(ClassGroup=(AttackHandler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UAbstractAttackHandler : public UActorComponent, public IAttackIssuer
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbstractAttackHandler();

	TScriptInterface<IDamageProvider>& Weapon;

	TArray<TScriptInterface<IDamageModifier>&> Modifiers;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual FHurtResult IssueAttack(
		TScriptInterface<IAttackInstigator>& Instigator,
		TScriptInterface<IHurtReceiver>& Receiver,
		TScriptInterface<IDamageModifier>& Skill,
		bool IsCritical = false) override;
};
