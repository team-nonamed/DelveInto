// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSkillSlot.h"
#include "Engine/DataAsset.h"
#include "NewSystem/Skills/SkillBase.h"
#include "WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class DELVEINTO_API UWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FText WeaponDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	UTexture2D* WeaponIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float BaseAttackSpeed = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skills")
	TMap<EWeaponSkillSlot, TSubclassOf<USkillBase>> SkillClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Skills")
	TObjectPtr<UPaperFlipbook> IdleFlipbook;
};
