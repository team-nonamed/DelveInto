// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Interfaces/Damages/DamageModifier.h"
#include "Types/ItemCategory.h"
#include "SkillData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DELVEINTO_API USkillData : public UPrimaryDataAsset, public IDamageModifier
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	TSet<EWeaponCategory> AllowedWeaponCategory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Information)
	uint8 MaxLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Attack)
	float Additive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Attack)
	float MultiplierAdditive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Attack)
	float CooldownSeconds;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cancel Config")
	bool IsCancelled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cancel Config")
	uint8 CancelPriority;

	virtual uint8 GetMaxLevel() const { return MaxLevel; }
	virtual bool GetCancelled() const override;
	virtual float GetDamageAdditive() const override;
	virtual float GetDamageMultiplierAdditive() const override;
	virtual uint8 GetPriority() const override;
	virtual float GetCooldownSeconds() const;
};
