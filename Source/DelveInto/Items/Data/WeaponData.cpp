// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponData.h"

UWeaponData::UWeaponData()
{
	Category = EItemCategory::Weapon;
}

float UWeaponData::GetBaseDamage() const
{
	return AttackDamage;
}

EWeaponCategory UWeaponData::GetWeaponCategory() const
{
	return WeaponCategory;
}

ERangeShape UWeaponData::GetRangeShape() const
{
	return RangeShape;
}

float UWeaponData::GetRange() const
{
	return Range;
}