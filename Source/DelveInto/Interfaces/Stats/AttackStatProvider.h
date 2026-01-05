// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackStatProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttackStatProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DELVEINTO_API IAttackStatProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 현재 들고있는 무기의 피해량을 제공하는 Method
	 * @return 현재 들고 있는 무기의 피해량
	 */
	virtual float GetWeaponDamage() const = 0;

	/**
	 * 현재 기본 공격시 피해량을 제공하는 Method
	 * @return 현재 기본 공격 피해량
	 */
	virtual float GetBaseAttackDamage() const = 0;

	/**
	 * 현재 기본 공격 스탯을 제공하는 Method
	 * @return 현재 기본 공격 스탯
	 */
	virtual float GetCurrentAttackStat() const = 0;
};
