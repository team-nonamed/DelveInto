#pragma once
#include "CoreMinimal.h"
#include "NewSystem/Enhances/PerkEffectBase.h"
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h" // EStatCategory를 위해 포함
#include "StatModifier.generated.h"

UCLASS(DisplayName = "Effect: Modify Base Stat")
class DELVEINTO_API UPerkEffect_StatModifier : public UPerkEffectBase
{
	GENERATED_BODY()

public:
	virtual void OnApplied(UPerkHandler* Handler, int32 PerkLevel) override;
	virtual void OnRemoved(UPerkHandler* Handler) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	EStatCategory TargetStat;

	int32 CurrentLevel = 0;
};