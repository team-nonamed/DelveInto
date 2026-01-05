// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Entity.generated.h"

UCLASS(Abstract)
class DELVEINTO_API AEntity : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void HandleMove(FVector2D DeltaMove);

	//virtual void HandleLook(FVector2D DeltaLook);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

};
