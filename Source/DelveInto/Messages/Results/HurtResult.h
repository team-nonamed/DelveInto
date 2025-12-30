#pragma once
#include "Types/ResultType.h"

#include "HurtResult.generated.h"

USTRUCT(BlueprintType)
struct FHurtResult
{
	GENERATED_BODY()

	UPROPERTY(Transient, Category="Hurt | Entities")
	TScriptInterface<class IHurtInitiator> Sender;

	UPROPERTY(Transient, Category="Hurt | Entities")
	TScriptInterface<class IHurtReceiver> Receiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float OriginalHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float OriginalDecreaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float AdditiveRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float MultiplicativeRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float TotalDecreaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Effects")
	float Knockback;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Meta")
	EResultType Result;

	FHurtResult()
		: Sender(nullptr)
		, Receiver(nullptr)
		, OriginalHealth(0.0f)
		, OriginalDecreaseRate(0.0f)
		, AdditiveRate(0.0f)
		, MultiplicativeRate(0.0f)
		, TotalDecreaseRate(0.0f)
		, Knockback(0.0f)
		, Result(EResultType::Unknown)
	{
	}

	FHurtResult(const EResultType Result)
		: Sender(nullptr)
		, Receiver(nullptr)
		, OriginalHealth(0.0f)
		, OriginalDecreaseRate(0.0f)
		, AdditiveRate(0.0f)
		, MultiplicativeRate(0.0f)
		, TotalDecreaseRate(0.0f)
		, Knockback(0.0f)
		, Result(Result)
	{
	}

	FHurtResult(
		const TScriptInterface<IHurtInitiator>& InSender,
		const TScriptInterface<IHurtReceiver>& InReceiver,
		const float InOriginalHealth,
		const float InOriginalDecreaseRate,
		const float InAdditiveRate,
		const float InMultiplicativeRate,
		const float InTotalDecreaseRate,
		const float InKnockback,
		const EResultType InResult)
		: Sender(InSender)
		, Receiver(InReceiver)
		, OriginalHealth(InOriginalHealth)
		, OriginalDecreaseRate(InOriginalDecreaseRate)
		, AdditiveRate(InAdditiveRate)
		, MultiplicativeRate(InMultiplicativeRate)
		, TotalDecreaseRate(InTotalDecreaseRate)
		, Knockback(InKnockback)
		, Result(InResult)
	{
	}
			
};
