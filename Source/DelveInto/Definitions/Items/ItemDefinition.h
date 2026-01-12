// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Definitions/Features/Attacks/AttackFeatureDefinition.h"
#include "Definitions/Features/Consumptions/ConsumptionFeatureDefinition.h"
#include "ItemDefinition.generated.h"

/**
 * 
 */
UCLASS()
class DELVEINTO_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	int MaxStack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Feature)
	TObjectPtr<UAttackFeatureDefinition> DefaultAttackFeature;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Feature)
	TObjectPtr<UConsumptionFeatureDefinition> DefaultConsumptionFeature;
};
