// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Features/Attacks/AttackFeature.h"
#include "Features/Consumptions/ConsumptionFeature.h"
#include "Interfaces/DescriptionProvider.h"
#include "Interfaces/IconProvider.h"
#include "Interfaces/NameProvider.h"
#include "AbstractItemDefinition.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class DELVEINTO_API UAbstractItemDefinition : public UPrimaryDataAsset,
	public INameProvider,
	public IDescriptionProvider,
	public IIconProvider
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=General)
	int32 MaxStack = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Feature)
	TObjectPtr<UConsumptionFeature> ConsumptionFeature;

	// TODO: 도구 파괴?

public:
	FPrimaryAssetId GetItemId() const { return GetPrimaryAssetId(); }
};
