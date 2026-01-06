// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInputController.h"

#include "EnhancedInputComponent.h"

void APlayerInputController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent || InputActions.IsEmpty())
	{
		return;
	}

	for (const TPair<const UInputAction*, EInputSignalType> InputAction: InputActions)
	{
		//EnhancedInputComponent->BindAction(InputAction.Key, ETriggerEvent::Started)
	}
}
