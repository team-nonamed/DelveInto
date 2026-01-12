// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/Features/AbstractAttackFeatureDefinition.h"
#include "AttackFeatureInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DELVEINTO_API UAttackFeatureInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UAbstractAttackFeatureDefinition> Definition;
	
	void Init(UAbstractAttackFeatureDefinition* Definition);
};
