#pragma once

#include "CoreMinimal.h"
#include "LimitCounts.generated.h"

USTRUCT(BlueprintType)
struct FLimitCounts
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount;

	void Validate()
	{
		if (MaxCount < MinCount)
		{
			MaxCount = MinCount;
		}
	}
};