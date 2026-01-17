#include "Input/PlayerInputController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

#include "Input/InputSignalConfig.h"
#include "Interfaces/Input/CombatInputReceiver.h"

void APlayerInputController::BeginPlay()
{
	Super::BeginPlay();

	// IMC 런타임 관리: LocalPlayer Subsystem에 추가
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const UInputMappingContext* IMC : DefaultIMCs)
			{
				if (IMC)
				{
					Subsys->AddMappingContext(IMC, /*Priority*/0);
				}
			}
		}
	}
}

void APlayerInputController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC || !InputSignalConfig)
	{
		return;
	}

	// config에 등장하는 Action들을 수집(중복 방지)
	TSet<const UInputAction*> UniqueActions;
	for (const auto& Pair : InputSignalConfig->SkillSignals)
	{
		if (Pair.Key.Action)
		{
			UniqueActions.Add(Pair.Key.Action.Get());
		}
	}

	// 각 Action에 대해 필요한 TriggerEvent만 바인딩
	for (const UInputAction* Action : UniqueActions)
	{
		// Started
		{
			FActionSignal Key;
			Key.Action = Action;
			Key.TriggerEvent = ETriggerEvent::Started;
			if (InputSignalConfig->SkillSignals.Contains(Key))
			{
				EIC->BindAction(Action, ETriggerEvent::Started, this, &APlayerInputController::OnActionStarted);
			}
		}

		// Completed
		{
			FActionSignal Key;
			Key.Action = Action;
			Key.TriggerEvent = ETriggerEvent::Completed;
			if (InputSignalConfig->SkillSignals.Contains(Key))
			{
				EIC->BindAction(Action, ETriggerEvent::Completed, this, &APlayerInputController::OnActionCompleted);
			}
		}

		// Canceled
		{
			FActionSignal Key;
			Key.Action = Action;
			Key.TriggerEvent = ETriggerEvent::Canceled;
			if (InputSignalConfig->SkillSignals.Contains(Key))
			{
				EIC->BindAction(Action, ETriggerEvent::Canceled, this, &APlayerInputController::OnActionCanceled);
			}
		}
	}
}

void APlayerInputController::OnActionStarted(const FInputActionInstance& Instance)
{
	Dispatch(Instance.GetSourceAction(), ETriggerEvent::Started);
}

void APlayerInputController::OnActionCompleted(const FInputActionInstance& Instance)
{
	Dispatch(Instance.GetSourceAction(), ETriggerEvent::Completed);
}

void APlayerInputController::OnActionCanceled(const FInputActionInstance& Instance)
{
	Dispatch(Instance.GetSourceAction(), ETriggerEvent::Canceled);
}

void APlayerInputController::Dispatch(const UInputAction* Action, ETriggerEvent TriggerEvent)
{
	if (!Action || !InputSignalConfig) return;

	FActionSignal Key;
	Key.Action = Action;
	Key.TriggerEvent = TriggerEvent;

	const FSkillCommand* Cmd = InputSignalConfig->SkillSignals.Find(Key);
	if (!Cmd) return;

	APawn* P = GetPawn();
	if (!P) return;

	// 가상함수 기반 인터페이스 호출
	if (ICombatInputReceiver* Receiver = Cast<ICombatInputReceiver>(P))
	{
		Receiver->HandleSkillInput(Cmd->Designator, TriggerEvent);
	}
}
