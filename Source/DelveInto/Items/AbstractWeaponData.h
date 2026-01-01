// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractItemData.h"
#include "Interfaces/Damages/DamageProvider.h"
#include "AbstractWeaponData.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class DELVEINTO_API UAbstractWeaponData : public UAbstractItemData, public IDamageProvider
{
	GENERATED_BODY()

	UAbstractWeaponData();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Weapon)
	
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Weapon)
	float AttackDamage;

public:
	virtual float GetBaseDamage() const override;
};
