#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Items/ItemEffect.h"
#include "Heal.generated.h"

UCLASS()
class DELVEINTO_API UItemEffect_Heal : public UItemEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float HealAmount = 50.0f;

	virtual void ApplyEffect_Implementation(AActor* Target) override;
};