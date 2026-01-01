#pragma once

UENUM(BlueprintType)
enum class EItemCategory: uint8
{
	Weapon,
	Equipment,
	Consumable,
};

UENUM(BlueprintType)
enum class EWeaponCategory: uint8
{
	Melee,
	Ranged
};