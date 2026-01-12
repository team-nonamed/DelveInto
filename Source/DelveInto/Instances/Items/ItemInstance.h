// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiffResults.h"
#include "IDetailTreeNode.h"
#include "ItemDefinition.h"
#include "Definitions/Items/ItemDefinition.h"
#include "Instances/Features/AbstractAttackFeatureInstance.h"
#include "Instances/Features/Attacks/AttackFeatureInstance.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "ItemInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DELVEINTO_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	void Init(const UItemDefinition* InDef);
	
	UPROPERTY()
	TObjectPtr<const UItemDefinition> Definition;

	UPROPERTY()
	int StackCount;

	UPROPERTY(Transient)
	TObjectPtr<UAttackFeatureInstance> AttackFeature;

	
};
