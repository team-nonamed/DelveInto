// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractWeaponData.h"

UAbstractWeaponData::UAbstractWeaponData()
{
	Category = EItemCategory::Weapon;
}

float UAbstractWeaponData::GetBaseDamage() const
{
	return AttackDamage;
}


