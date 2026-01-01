// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractAttackHandler.h"

#include "Messages/InnerResult.h"
#include "TraceServices/Model/TableImport.h"


// Sets default values for this component's properties
UAbstractAttackHandler::UAbstractAttackHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbstractAttackHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAbstractAttackHandler::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FHurtResult UAbstractAttackHandler::IssueAttack(
	TScriptInterface<IAttackInstigator>& Instigator,
	TScriptInterface<IHurtReceiver>& Receiver,
	TScriptInterface<IDamageModifier>& Skill,
	const bool IsCritical)
{
	if (!Receiver)
	{
		return FHurtResult(EResultType::Invalid);
	}

	if (!Skill)
	{
		return FHurtResult(EResultType::Invalid);
	}

	// 공격자가 들고 있는 무기와 사용한 스킬의 피해량 증감 계산
	float BaseDamage = Weapon->GetBaseDamage();
	BaseDamage *= 1 + Skill->GetDamageMultiplierDelta();
	BaseDamage += Skill->GetDamageAdditive();
	
	if (BaseDamage <= DBL_EPSILON)
	{
		return FHurtResult(EResultType::Impossible);
	}

	float CurrentAdditive = 0.0f;
	float CurrentMultiplier = 1.0f;

	FInnerResult InnerResult = FInnerResult();
	
	for (TScriptInterface<IDamageModifier>& Modifier : Modifiers)
	{
		if (!Modifier)
		{
			continue;
		}

		CurrentAdditive += Modifier->GetDamageAdditive();
		CurrentMultiplier += Modifier->GetDamageMultiplierDelta();

		InnerResult.ApplyModifier(Modifier->GetCancelled(), Modifier->GetPriority());
	}

	if (InnerResult.IsCancelled())
	{
		return FHurtResult(EResultType::Cancelled);
	}

	float CurrentDamage = BaseDamage * CurrentMultiplier + CurrentAdditive;
	
	if (CurrentDamage <= DBL_EPSILON)
	{
		return FHurtResult(EResultType::Impossible);
	}

	const FHurtRequest Request = FHurtRequest(Instigator, Receiver, CurrentDamage, IsCritical);

	return Receiver->ReceiveHurt(Request);
}


