#pragma once

UENUM(BlueprintType)
enum class ERangeShape: uint8
{
	NearBy,
	CircularSector,
	Circle,
};

UENUM(BlueprintType)
enum class EAttackDelivery: uint8
{
	MeleeArea,
	Projectile,
};

UENUM(BlueprintType)
enum class ESkillDesignator: uint8
{
	Base,
	First,
	Second
};
