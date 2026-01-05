// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryHandler.h"


// Sets default values for this component's properties
UInventoryHandler::UInventoryHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



// Called every frame
void UInventoryHandler::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TWeakInterfacePtr<IDamageProvider> UInventoryHandler::GetActivatedWeapon() const
{
	return Weapons[ActivatedWeaponIndex];
}

