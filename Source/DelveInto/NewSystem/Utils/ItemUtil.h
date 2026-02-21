// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbook.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NewSystem/Items/ItemInstance.h"
#include "ItemUtil.generated.h"

/**
 * 프로젝트 전역에서 사용할 정적 헬퍼 함수 모음
 */
UCLASS()
class DELVEINTO_API UItemUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Item | Spawn")
	static AItemInstance* SpawnDroppedItem(const UObject* WorldContextObject, FGameplayTag ItemIDToSpawn, int32 Amount, FVector SpawnLocation);
};
