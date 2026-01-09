// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputSignalConfig.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputSignalConfig> InputSignalConfig;

	virtual void SetupInputComponent() override;

protected:
	void OnActionStarted(const FInputActionInstance& Instance);
	void OnActionOngoing(const FInputActionInstance& Instance);
	void OnActionTriggered(const FInputActionInstance& Instance);
	void OnActionCompleted(const FInputActionInstance& Instance);
	void OnActionCanceled(const FInputActionInstance& Instance);

private:
	void DispatchSignal(ETriggerEvent Event, const FInputActionInstance& Instance); // ← 이게 빠지면 “없다”가 납니다.
	void HandleSignal(EInputSignalType SignalType, const FInputActionInstance& Instance);
};