// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AbstractItemData.h"
#include "Interfaces/Damages/DamageProvider.h"
#include "Types/RangeShape.h"
#include "WeaponData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DELVEINTO_API UWeaponData : public UAbstractItemData, public IDamageProvider
{
	GENERATED_BODY()

	UWeaponData();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Information")
	EWeaponCategory WeaponCategory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Attack")
	float AttackDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Attack")
	ERangeShape RangeShape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Attack")
	float Range;

public:
	virtual float GetBaseDamage() const override;

	virtual EWeaponCategory GetWeaponCategory() const;

	virtual ERangeShape GetRangeShape() const;

	virtual float GetRange() const;
	
};
