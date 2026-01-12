// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/Features/AbstractAttackFeatureDefinition.h"
#include "Engine/DataAsset.h"
#include "Types/Items.h"
#include "AttackFeatureDefinition.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DELVEINTO_API UAttackFeatureDefinition : public UAbstractAttackFeatureDefinition
{
	GENERATED_BODY()

	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponCategory Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRequireLOS;

};
