#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"

#include "PlayerInputController.generated.h"

class UInputSignalConfig;
class UInputMappingContext;
struct FInputActionInstance;

/**
 * Enhanced Input 기반 Input Controller.
 * - UInputSignalConfig에 정의된 Action+TriggerEvent를 바인딩
 * - 발생한 이벤트를 Possessed Pawn(ICombatInputReceiver)에게 전달
 */
UCLASS()
class DELVEINTO_API APlayerInputController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputSignalConfig> InputSignalConfig;

	// (선택) 시작 시 추가할 IMC 목록
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TArray<TObjectPtr<const UInputMappingContext>> DefaultIMCs;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

protected:
	// TriggerEvent별 콜백(Instance로 Action을 식별)
	void OnActionStarted(const FInputActionInstance& Instance);
	void OnActionCompleted(const FInputActionInstance& Instance);
	void OnActionCanceled(const FInputActionInstance& Instance);

	void Dispatch(const UInputAction* Action, ETriggerEvent TriggerEvent);
};
