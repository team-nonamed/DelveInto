// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DefaultEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class DELVEINTO_API ADefaultEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADefaultEnemyAIController();

protected:
	UPROPERTY(EditAnywhere, Category="AI")
		FName TargetActorTag = TEXT("Player");

	UPROPERTY(EditAnywhere, Category="AI")
		float SearchStartRadius = 1000.0f;

	UPROPERTY(VisibleAnywhere, Category="AI")
		ACharacter* CurrentTarget = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void UpdateTargetByTag();
	void UpdateMovementTowardsTarget();
};
