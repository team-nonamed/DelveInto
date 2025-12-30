// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractHealthHandler.h"

#include "VectorUtil.h"
#include "Messages/InnerResult.h"


// Sets default values for this component's properties
UAbstractHealthHandler::UAbstractHealthHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbstractHealthHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAbstractHealthHandler::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FHurtResult UAbstractHealthHandler::HandleHurt(const FHurtRequest& Request)
{
	float CurrentAdditive = 0.0f;
	float CurrentMultiplicative = 1.0f;

	FInnerResult InnerResult = FInnerResult();

	for (const TScriptInterface<IHurtModifier>& Modifier : Modifiers)
	{
		if (!Modifier)
		{
			continue;
		}
		
		CurrentAdditive += Modifier->GetHurtAdditive();
		CurrentMultiplicative += (Modifier->GetHurtMultiplicative()-1);

		InnerResult.ApplyModifier(Modifier->GetCancelled(), Modifier->GetPriority());
	}

	if (InnerResult.IsCancelled())
	{
		return FHurtResult(EResultType::Cancelled);
	}
	
	const float CurrentDamage = Request.Damage * CurrentMultiplicative + CurrentAdditive;

	if (CurrentDamage <= 0.0f)
	{
		return FHurtResult(EResultType::Nullified);
	}

	

	FHurtResult Result = FHurtResult();

	Result.Sender = Request.Sender;
	Result.Receiver = Request.Receiver;
	Result.AdditiveRate = CurrentAdditive;
	Result.MultiplicativeRate = CurrentMultiplicative;
	Result.OriginalHealth = CurrentHealth;
	Result.OriginalDecreaseRate = Request.Damage;
	Result.Knockback = 0.0f;
	Result.TotalDecreaseRate = CurrentDamage;
	Result.Result = EResultType::Success;

	CurrentHealth = FMath::Clamp(CurrentHealth - CurrentDamage, 0.0f, MaxHealth);
	OnHurt.Broadcast(Result);

	if (CurrentHealth <= 0.0f)
	{
		OnDeath.Broadcast(Result);
	}

	return Result;
}
