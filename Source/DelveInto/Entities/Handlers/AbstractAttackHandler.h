// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Attacks/AttackIssuer.h"
#include "Interfaces/Damages/DamageModifier.h"
#include "Interfaces/Damages/DamageProvider.h"
#include "Interfaces/Skills/SkillProvider.h"
#include "Interfaces/Stats/AttackStatProvider.h"
#include "Inventories/InventoryHandler.h"
#include "Items/ItemInstance.h"
#include "AbstractAttackHandler.generated.h"


UCLASS(ClassGroup=(AttackHandler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UAbstractAttackHandler : public UActorComponent, public IAttackIssuer, public IAttackStatProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbstractAttackHandler();

	TWeakInterfacePtr<ISkillProvider> Skills;

	TArray<TScriptInterface<IDamageModifier>> Modifiers;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInventoryHandler> Inventory;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Attack;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual FHurtResult IssueAttack(
		ESkillDesignator Designator) override;

	virtual float GetWeaponDamage() const override;
	virtual float GetCurrentAttackStat() const override;
	virtual float GetBaseAttackDamage() const override;

	
	
};
