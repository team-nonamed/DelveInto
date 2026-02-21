#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemEffect.generated.h"

/**
 * 아이템을 사용했을 때 발동할 효과들의 최상위 부모 클래스
 * EditInlineNew: 블루프린트 에디터 안에서 이 클래스를 즉석으로 생성/편집할 수 있게 해줌! (핵심)
 */
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class DELVEINTO_API UItemEffect : public UObject
{
	GENERATED_BODY()

public:
	// 타겟(주로 플레이어)에게 효과를 적용하는 함수
	// BlueprintNativeEvent로 만들어 C++과 블루프린트 양쪽에서 맘대로 구현 가능하게 합니다.
	UFUNCTION(BlueprintNativeEvent, Category = "Item Effect")
	void ApplyEffect(AActor* Target);
	virtual void ApplyEffect_Implementation(AActor* Target);
};