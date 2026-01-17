#include "Items/InventoryHandler.h"
#include "Items/ItemInstance.h"
#include "Items/ItemDefinition.h"

UInventoryHandler::UInventoryHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UItemInstance* UInventoryHandler::GetActivatedWeapon() const
{
	if (!Weapons.IsValidIndex(ActivatedWeaponIndex))
	{
		return nullptr;
	}
	return Weapons[ActivatedWeaponIndex];
}

UItemInstance* UInventoryHandler::AddWeapon(UAbstractItemDefinition* WeaponDef)
{
	if (!WeaponDef) return nullptr;

	UItemInstance* Inst = NewObject<UItemInstance>(this);
	Inst->InitFromDefinition(WeaponDef);
	Weapons.Add(Inst);

	// 첫 무기면 자동 장착
	if (ActivatedWeaponIndex == INDEX_NONE)
	{
		ActivatedWeaponIndex = 0;
	}

	return Inst;
}

void UInventoryHandler::SetActivatedWeaponIndex(int32 NewIndex)
{
	if (Weapons.IsValidIndex(NewIndex))
	{
		ActivatedWeaponIndex = NewIndex;
	}
}
