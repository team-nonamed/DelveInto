// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "Types/InputSignalType.h"
#include "PlayerInputController.generated.h"

/**
 * 유저의 입력을 처리하는 Class
 */
UCLASS()
class DELVEINTO_API APlayerInputController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<const UInputAction*, EInputSignalType> InputActions;

	virtual void SetupInputComponent() override;
};
