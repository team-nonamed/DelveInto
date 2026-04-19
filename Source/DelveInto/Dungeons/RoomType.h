#pragma once

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Normal,
	Start,
	Boss,
	Treasure,
	NPC
};

UENUM(BlueprintType)
enum class ERoomVisitStatus: uint8
{
	Unknown,
	Unvisited,
	Visited,
};