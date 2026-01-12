// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInstance.h"

void UItemInstance::Init(const UItemDefinition* InDef)
{
	Definition = InDef;
	check(Definition);

	UAttackFeatureDefinition* AttackDefinition = Definition->DefaultAttackFeature;
	check(AttackDefinition)
	
	AttackFeature = NewObject<UAttackFeatureInstance>(this);
	AttackFeature->Init(AttackDefinition);

	//TODO: Consumption Feature
}
