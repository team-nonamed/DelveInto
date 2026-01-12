// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Instances/Items/ItemInstance.h"
#include "InventoryHandler.generated.h"


class UItemInstance;

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UInventoryHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryHandler();

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TObjectPtr<const UItemInstance>> Weapons;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int ActivatedWeaponIndex;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual const UItemInstance* GetActivatedWeapon() const;

	int SetActivatedWeaponIndex(const int Index);

	
};
