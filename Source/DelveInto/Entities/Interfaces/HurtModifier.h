#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HurtModifier.generated.h"

UINTERFACE(BlueprintType)
class UHurtModifier: public UInterface
{
	GENERATED_BODY()
};

class DELVEINTO_API IHurtModifier
{
	GENERATED_BODY()

public:
	/**
	 * 추가로 더해질 피해량을 제공하는 Method
	 * @return 추가로 더해질 피해량, 기본 값은 보통 0
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageModifier")
	float GetHurtAdditive() const;

	/**
	 * 추가로 곱해질 피해량 배율을 제공하는 Method
	 * @return 추가로 곱해질 피해량 배율, 기본 값은 보통 1
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageModifier")
	float GetHurtMultiplier();
};
