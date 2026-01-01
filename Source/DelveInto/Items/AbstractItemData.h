// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Interfaces/DescriptionProvider.h"
#include "Interfaces/IconProvider.h"
#include "Interfaces/NameProvider.h"
#include "Types/ItemCategory.h"
#include "AbstractItemData.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class DELVEINTO_API UAbstractItemData : public UPrimaryDataAsset,
	public INameProvider,
	public IDescriptionProvider,
	public IIconProvider
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	EItemCategory Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=General)
	int32 MaxStack = 1;

public:
	FPrimaryAssetId GetItemId() const;
	
	virtual FText GetCurrentName() override;

	virtual FText GetCurrentDescription() override;

	virtual TObjectPtr<const UTexture2D> GetCurrentIcon() override;
};
