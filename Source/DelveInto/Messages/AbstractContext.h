#pragma once
#include "AbstractMessage.h"

USTRUCT()
struct FAbstractContext: public FAbstractMessage
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category="Interaction")
	bool bIsCancelled = true;
	
};