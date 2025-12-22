#pragma once

#include "MoveIntentSender.generated.h"

UINTERFACE(BlueprintType)
class UMoveIntentSender: public UInterface
{
	GENERATED_BODY()
};

class DELVEINTO_API IMoveIntentSender
{
	GENERATED_BODY()

public:
	/**
	 * Move Signal과 Intention Injection을 매개하는 Method
	 * @param SinkOwner Intent가 들어갈 Sink의 Owner
	 * @param DeltaMove 움직이는 양
	 * @return 성공 여부
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Intention")
	bool PushMoveIntent(AActor* SinkOwner, const FVector2D& DeltaMove);

	virtual bool PushMoveIntent_Implementation(TWeakObjectPtr<AActor> SinkOwner, const FVector2D& DeltaMove)
	{
		return false;
	}
};

