#pragma once
#include "Types/Hurts.h"

#include "HurtResult.generated.h"

USTRUCT(BlueprintType)
struct FHurtResult
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TScriptInterface<class IAttackInstigator> Instigator;

	UPROPERTY(Transient)
	TScriptInterface<class IHurtReceiver> Receiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float OriginalHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float IncomingDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float AdditiveDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float DamageMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float FinalDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Effects")
	float Knockback;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Meta")
	EResultType Result;

	FHurtResult()
		: Instigator(nullptr)
		, Receiver(nullptr)
		, OriginalHealth(0.0f)
		, IncomingDamage(0.0f)
		, AdditiveDamage(0.0f)
		, DamageMultiplier(0.0f)
		, FinalDamage(0.0f)
		, Knockback(0.0f)
		, Result(EResultType::Unknown)
	{
	}

	FHurtResult(const EResultType Result)
		: Instigator(nullptr)
		, Receiver(nullptr)
		, OriginalHealth(0.0f)
		, IncomingDamage(0.0f)
		, AdditiveDamage(0.0f)
		, DamageMultiplier(0.0f)
		, FinalDamage(0.0f)
		, Knockback(0.0f)
		, Result(Result)
	{
	}

	FHurtResult(
		const TScriptInterface<IAttackInstigator>& InInstigator,
		const TScriptInterface<IHurtReceiver>& InReceiver,
		const float InOriginalHealth,
		const float InOriginalDecreaseRate,
		const float InAdditiveRate,
		const float InMultiplicativeRate,
		const float InTotalDecreaseRate,
		const float InKnockback,
		const EResultType InResult)
		: Instigator(InInstigator)
		, Receiver(InReceiver)
		, OriginalHealth(InOriginalHealth)
		, IncomingDamage(InOriginalDecreaseRate)
		, AdditiveDamage(InAdditiveRate)
		, DamageMultiplier(InMultiplicativeRate)
		, FinalDamage(InTotalDecreaseRate)
		, Knockback(InKnockback)
		, Result(InResult)
	{
	}
			
};
