// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Entities/AbstractHost.h"
#include "Messages/Results/HurtResult.h"
#include "Skills/Instances/SkillInstance.h"
#include "Types/ItemCategory.h"
#include "AbstractAttackFeatureDefinition.generated.h"

/**
 * 
 */
UCLASS(Abstract, ClassGroup=(Attack), BlueprintType)
class DELVEINTO_API UAbstractAttackFeatureDefinition : public UDataAsset
{
	GENERATED_BODY()

protected:
	virtual EWeaponCategory GetWeaponCategory() PURE_VIRTUAL(UAbstractAttackFeatureDefinition::GetWeaponCategory, return EWeaponCategory::None; );

	virtual 
};
