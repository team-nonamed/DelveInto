// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillData.h"

bool USkillData::GetCancelled() const
{
	return IsCancelled;
}

float USkillData::GetDamageAdditive() const
{
	return Additive;
}

float USkillData::GetDamageMultiplierAdditive() const
{
	return MultiplierAdditive;
}

uint8 USkillData::GetPriority() const
{
	return CancelPriority;
}

float USkillData::GetCooldownSeconds() const
{
	return CooldownSeconds;
}

FPrimaryAssetId USkillData::GetPrimaryId() const
{
	return GetPrimaryAssetId();
}


