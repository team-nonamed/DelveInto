#pragma once

#include "HurtContext.generated.h"

USTRUCT(BlueprintType)
struct FHurtContext
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TWeakObjectPtr<AActor> Sender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TWeakObjectPtr<AActor> Receiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float BaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float AdditiveRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float MultiplierRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Effects")
	float Knockback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurt | Meta")
	bool IsCancelled;
};
