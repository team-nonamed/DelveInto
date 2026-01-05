// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractHealthHandler.h"

#include "Messages/InnerResult.h"


// Sets default values for this component's properties
UAbstractHealthHandler::UAbstractHealthHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UAbstractHealthHandler::BeginPlay()
{
	Super::BeginPlay();
}

FHurtResult UAbstractHealthHandler::HandleHurt(const FHurtRequest& Request)
{
	UE_LOG(LogActor, Display, TEXT("Handle Hurt"))
	
#pragma region 추가 피해량 계산
	float CurrentAdditive = 0.0f;
	float CurrentMultiplicative = 1.0f;

	FInnerResult InnerResult = FInnerResult();

	for (const TScriptInterface<IDamageModifier>& Modifier : Modifiers)
	{
		if (!Modifier)
		{
			continue;
		}
		
		CurrentAdditive += Modifier->GetDamageAdditive();
		CurrentMultiplicative += Modifier->GetDamageMultiplierAdditive();

		InnerResult.ApplyModifier(Modifier->GetCancelled(), Modifier->GetPriority());
	}

#pragma endregion
	
	if (InnerResult.IsCancelled())
	{
		return FHurtResult(EResultType::Cancelled);
	}
	
	const float CurrentDamage = Request.Damage * CurrentMultiplicative + CurrentAdditive;

	if (CurrentDamage <= DBL_EPSILON)
	{
		
		return FHurtResult(EResultType::Nullified);
	}

	FHurtResult Result = FHurtResult(
		Request.Instigator,
		Request.Receiver,
		CurrentHealth,
		Request.Damage,
		CurrentAdditive,
		CurrentMultiplicative,
		CurrentDamage,
		0.0f,
		EResultType::Success
		);

	CurrentHealth = FMath::Clamp(CurrentHealth - CurrentDamage, 0.0f, MaxHealth);
	OnHurt.Broadcast(Result);

	if (!IsDead && CurrentHealth <= 0.0f)
	{
		OnDeath.Broadcast(Result);
	}

	return Result;
}
