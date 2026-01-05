// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillInstance.h"

void USkillInstance::Init(const TObjectPtr<USkillData> InData)
{
	Data = InData;
}

bool USkillInstance::CanActivate(const TObjectPtr<UWorld> World) const
{
	if (!Data) return false;
	return !IsOnCooldown(World);
}

bool USkillInstance::IsOnCooldown(const TObjectPtr<UWorld> World) const
{
	if (!World) return false;
	return World->GetTimeSeconds() < CooldownEndTime;
}

bool USkillInstance::TryActivate(const TObjectPtr<UWorld> World)
{
	if (!World || !Data) return false;
	if (!CanActivate(World)) return false;

	// TODO: 실제 스킬 효과 적용

	const float Now = World->GetTimeSeconds();
	CooldownEndTime = Now + Data -> CooldownSeconds;
	return true;
}

float USkillInstance::GetDamageAdditive() const
{
	return Data->GetDamageAdditive();
}

float USkillInstance::GetDamageMultiplierAdditive() const
{
	return Data->GetDamageMultiplierAdditive();
}

bool USkillInstance::GetCancelled() const
{
	return Data -> GetCancelled();
}

uint8 USkillInstance::GetPriority() const
{
	return Data -> GetPriority();
}

