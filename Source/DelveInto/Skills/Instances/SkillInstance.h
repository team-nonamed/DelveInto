// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Damages/DamageProvider.h"
#include "Skills/Data/SkillData.h"
#include "UObject/Object.h"
#include "SkillInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DELVEINTO_API USkillInstance : public UObject, public IDamageModifier
{
	GENERATED_BODY()

public:
	void Init(const TObjectPtr<USkillData> InData);

	TObjectPtr<const USkillData> GetSkillData() const { return Data; }

	bool CanActivate(const TObjectPtr<UWorld> World) const;

	bool IsOnCooldown(const TObjectPtr<UWorld> World) const;

	bool TryActivate(const TObjectPtr<UWorld> World);

	virtual float GetDamageAdditive() const override;

	virtual float GetDamageMultiplierAdditive() const override;

	virtual bool GetCancelled() const override;

	virtual uint8 GetPriority() const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const USkillData> Data = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxLevel;

	float CooldownEndTime = 0.0f;
	
};
