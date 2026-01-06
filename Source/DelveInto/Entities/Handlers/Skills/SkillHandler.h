// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Entities/Handlers/AbstractSkillHandler.h"
#include "SkillHandler.generated.h"


UCLASS(ClassGroup=(Attack), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkillHandler : public UAbstractSkillHandler
{
	GENERATED_BODY()
};
