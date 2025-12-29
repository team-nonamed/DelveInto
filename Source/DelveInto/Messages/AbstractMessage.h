#pragma once
#include "Interfaces/Hurts/HurtInitiator.h"
#include "Types/InteractionType.h"


class IHurtReceiver;

USTRUCT(BlueprintType)
struct FAbstractMessage
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	EInteractionType InteractionType;
};