#pragma once

#include "HurtResult.generated.h"

USTRUCT(BlueprintType)
struct FHurtResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TWeakObjectPtr<AActor> Sender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TWeakObjectPtr<AActor> Receiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float OriginalHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float OriginalDecreaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float AdditiveRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float MultiplierRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float TotalDecreaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Effects")
	float Knockback;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt | Meta")
	bool IsCancelled;
};
