#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "ItemUtil.generated.h"

class AItemInstance;

UCLASS()
class DELVEINTO_API UItemUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @param LaunchImpulse 아이템이 튀어나갈 초기 힘의 방향과 세기입니다.
	 */
	static TArray<AItemInstance*> SpawnDroppedItems(
		const UObject* WorldContextObject, 
		FGameplayTag ItemIDToSpawn, 
		int32 Amount, 
		FVector SpawnLocation, 
		FVector LaunchImpulse = FVector::ZeroVector // [신규] 기본값은 0
	);
};