#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventoryHandler.generated.h"

class UItemInstance;
class UAbstractItemDefinition;

UCLASS(ClassGroup=(DelveInto), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UInventoryHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryHandler();

	UItemInstance* GetActivatedWeapon() const;

	// 예시: 정의로부터 무기 인스턴스를 생성해 추가
	UItemInstance* AddWeapon(UAbstractItemDefinition* WeaponDef);

	void SetActivatedWeaponIndex(int32 NewIndex);

protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UItemInstance>> Weapons;

	UPROPERTY(Transient)
	int32 ActivatedWeaponIndex = INDEX_NONE;
};
