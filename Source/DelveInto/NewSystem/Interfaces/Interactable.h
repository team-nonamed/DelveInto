#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 상호작용 가능한 액터들이 반드시 구현해야 하는 인터페이스입니다.
 */
class DELVEINTO_API IInteractable
{
	GENERATED_BODY()

public:
	// C++과 블루프린트 양쪽에서 맘대로 구현할 수 있는 만능 함수!
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);
};