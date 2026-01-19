#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DelveAIController.generated.h"

UCLASS()
class DELVEINTO_API ADelveAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	class APawn* PlayerPawn;
};