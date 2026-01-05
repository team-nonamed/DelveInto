#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageModifier.generated.h"

UINTERFACE(BlueprintType)
class UDamageModifier: public UInterface
{
	GENERATED_BODY()
};

class DELVEINTO_API IDamageModifier
{
	GENERATED_BODY()

public:
	/**
	 * 추가로 더해질 피해량을 제공하는 Method
	 * @return 추가로 더해질 피해량, 기본 값은 보통 0
	 */
	virtual float GetDamageAdditive() const = 0;

	/**
	 * 추가로 곱해질 피해량 배율을 제공하는 Method
	 * @return 추가로 곱해질 피해량 배율, 기본 값은 보통 0
	 */
	virtual float GetDamageMultiplierAdditive() const = 0;

	virtual bool GetCancelled() const = 0;

	virtual uint8 GetPriority() const = 0;
};
