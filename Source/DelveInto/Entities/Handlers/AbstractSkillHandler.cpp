// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractSkillHandler.h"

#include <stdexcept>


// Sets default values for this component's properties
UAbstractSkillHandler::UAbstractSkillHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbstractSkillHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

TObjectPtr<const USkillInstance> UAbstractSkillHandler::GetSkill(ESkillDesignator Designator) const
{
	if (!Skills.Contains(Designator))
	{
		throw new std::out_of_range("Skill not found");
	}

	return Skills[Designator];
}