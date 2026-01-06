// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "Engine/DataAsset.h"
#include "Types/InputSignalType.h"
#include "InputSignalConfig.generated.h"

USTRUCT(BlueprintType)
struct FActionSignal
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UInputAction> Action = nullptr;

	UPROPERTY(EditDefaultsOnly)
	ETriggerEvent TriggerEvent = ETriggerEvent::Started;

	friend bool operator==(const FActionSignal& A, const FActionSignal& B)
	{
		return A.Action == B.Action
			&& A.TriggerEvent == B.TriggerEvent;
	}
};

FORCEINLINE uint32 GetTypeHash(const FActionSignal& S)
{
	// UObject* 포인터는 GetTypeHash가 제공됩니다.
	const uint32 ActionHash = GetTypeHash(S.Action.Get());
	const uint32 TriggerHash = ::GetTypeHash(static_cast<uint8>(S.TriggerEvent));

	// HashCombine / HashCombineFast 둘 중 아무거나 사용 가능
	return HashCombine(ActionHash, TriggerHash);
}

/**
 * 
 */
UCLASS(BlueprintType)
class DELVEINTO_API UInputSignalConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FActionSignal, EInputSignalType> Signals;
};
