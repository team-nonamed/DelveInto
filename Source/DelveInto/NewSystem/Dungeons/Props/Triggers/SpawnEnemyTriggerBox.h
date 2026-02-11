// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "SpawnEnemyTriggerBox.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USpawnEnemyTriggerBox : public UBoxComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USpawnEnemyTriggerBox();

protected:
	virtual void OnRegister() override;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
