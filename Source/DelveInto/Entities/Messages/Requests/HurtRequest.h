#pragma once

#include "HurtRequest.generated.h"

USTRUCT(BlueprintType)
struct FHurtRequest
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TWeakObjectPtr<AActor> Sender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Entities")
	TWeakObjectPtr<AActor> Receiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurt | Rate")
	float BaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health | Effects")
	float Knockback;
};
