#pragma once

#include "CoreMinimal.h"
#include "HurtRequest.generated.h"

USTRUCT(BlueprintType)
struct FHurtRequest
{
	GENERATED_BODY()

	TScriptInterface<class IAttackInstigator> Instigator = nullptr;

	TScriptInterface<class IHurtReceiver> Receiver = nullptr;

	float Damage = 0.0f;

	bool IsCritical = false;

	FHurtRequest() = default;

	FHurtRequest(TScriptInterface<IAttackInstigator> InInstigator,
		TScriptInterface<IHurtReceiver> InReceiver,
		float InDamage,
		bool InIsCritical)
			: Instigator(InInstigator)
			, Receiver(InReceiver)
			, Damage(InDamage)
			, IsCritical(InIsCritical)
	{
	}
};
