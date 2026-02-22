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
	 * 지정된 개수(Amount)만큼 C++ AItemInstance를 1개 단위로 쪼개어 스폰합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item|Spawn", meta=(WorldContext="WorldContextObject"))
	static TArray<AItemInstance*> SpawnDroppedItems(
		const UObject* WorldContextObject, 
		FGameplayTag ItemIDToSpawn,  // [핵심] 클래스 지정 없이 태그만 받습니다!
		int32 Amount, 
		FVector SpawnLocation
	);
};