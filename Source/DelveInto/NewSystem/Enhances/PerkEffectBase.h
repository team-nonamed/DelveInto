#pragma once

#include "CoreMinimal.h"
#include "PerkEffectBase.generated.h"

class UPerkHandler;

/**
 * 퍽의 실제 효과를 담당하는 모듈입니다.
 * EditInlineNew를 통해 BP 에디터에서 인스턴싱하여 조립할 수 있습니다.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DELVEINTO_API UPerkEffectBase : public UObject
{
	GENERATED_BODY()

public:
	// 퍽 획득 시 호출 (델리게이트 바인딩 등)
	virtual void OnApplied(UPerkHandler* Handler, int32 PerkLevel);
    
	// 퍽 제거 시 호출 (델리게이트 언바인딩 등)
	virtual void OnRemoved(UPerkHandler* Handler);

protected:
	// 레벨(등급)에 따른 수치 데이터 (0: 일반, 1: 드문, 2: 희귀...)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TArray<float> EffectValues;

	// 현재 레벨에 맞는 수치를 안전하게 가져오는 함수
	UFUNCTION(BlueprintCallable, Category = "Effect")
	float GetValueForLevel(int32 Level) const;
};