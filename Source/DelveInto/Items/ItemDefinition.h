// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractItemDefinition.h"
#include "Engine/DataAsset.h"
#include "Features/Attacks/AttackFeature.h"
#include "Features/Consumptions/ConsumptionFeature.h"
#include "Interfaces/DescriptionProvider.h"
#include "Interfaces/IconProvider.h"
#include "Interfaces/NameProvider.h"
#include "ItemDefinition.generated.h"

/**
 * 
 */
UCLASS()
class DELVEINTO_API UItemDefinition : public UAbstractItemDefinition
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Features)
	TObjectPtr<UAttackFeature> AttackFeature;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Features)
	TObjectPtr<UConsumptionFeature> ConsumptionFeature;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=General)
	int32 MaxStack = 1;

public:
	
	virtual FText GetCurrentName() const override { return DisplayName; }

	virtual FText GetCurrentDescription() const override { return Description; }

	virtual TObjectPtr<const UTexture2D> GetCurrentIcon() const override { return Icon; }
};
