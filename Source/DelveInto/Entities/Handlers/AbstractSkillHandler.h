// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/SkillProvider.h"
#include "AbstractSkillHandler.generated.h"


UCLASS(Abstract)
class DELVEINTO_API UAbstractSkillHandler : public UActorComponent, public ISkillProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbstractSkillHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<ESkillDesignator, TObjectPtr<USkillInstance>> Skills;

public:
	virtual TObjectPtr<const USkillInstance> GetSkill(ESkillDesignator Designator) const override;
};
