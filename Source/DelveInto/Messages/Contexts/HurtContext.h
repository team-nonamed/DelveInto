#pragma once
#include "Messages/Cancellation.h"
#include "Messages/InnerResult.h"

#include "HurtContext.generated.h"

USTRUCT(BlueprintType)
struct FHurtContext
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TScriptInterface<class IHurtInitiator> Sender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TWeakObjectPtr<AActor> Receiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float BaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float AdditiveRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float MultiplicativeRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Effects")
	float Knockback;

	FInnerResult Cancellation = FInnerResult();

	FHurtContext()
		: Sender(nullptr)
		, Receiver(nullptr)
		, BaseRate(0.0f)
		, AdditiveRate(0.0f)
		, MultiplicativeRate(0.0f)
		, Knockback(0.0f)
	{}
};
