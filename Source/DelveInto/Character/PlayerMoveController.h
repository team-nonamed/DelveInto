// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "PlayerMoveController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class DELVEINTO_API APlayerMoveController : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerMoveController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputAction* SprintAction;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 650.f;

	void OnMove(const FInputActionValue& value);
	void OnLook(const FInputActionValue& value);
	void OnSprintStart(const FInputActionValue& value);
	void OnSprintEnd(const FInputActionValue& value);

	void SetCurrentSpeed(float NewSpeed);
};
