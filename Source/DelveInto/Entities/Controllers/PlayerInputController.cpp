// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInputController.h"

#include "EnhancedInputComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerInputController, Log, All);

void APlayerInputController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC)
	{
		UE_LOG(LogPlayerInputController, Error, TEXT("InputComponent is not UEnhancedInputComponent. Enhanced Input is not set up."));
		return;
	}

	if (!InputSignalConfig)
	{
		UE_LOG(LogPlayerInputController, Warning, TEXT("InputSignalConfig is null. No input bindings will be created."));
		return;
	}

	// Config에 정의된 (Action, TriggerEvent) 조합만 바인딩
	for (const TPair<FActionSignal, EInputSignalType>& Pair : InputSignalConfig->Signals)
	{
		const FActionSignal& Sig = Pair.Key;
		const UInputAction* Action = Sig.Action.Get();
		if (!Action)
		{
			continue;
		}

		switch (Sig.TriggerEvent)
		{
		case ETriggerEvent::Started:
			EIC->BindAction(Action, ETriggerEvent::Started, this, &APlayerInputController::OnActionStarted);
			break;

		case ETriggerEvent::Ongoing:
			EIC->BindAction(Action, ETriggerEvent::Ongoing, this, &APlayerInputController::OnActionOngoing);
			break;

		case ETriggerEvent::Triggered:
			EIC->BindAction(Action, ETriggerEvent::Triggered, this, &APlayerInputController::OnActionTriggered);
			break;

		case ETriggerEvent::Completed:
			EIC->BindAction(Action, ETriggerEvent::Completed, this, &APlayerInputController::OnActionCompleted);
			break;

		case ETriggerEvent::Canceled:
			EIC->BindAction(Action, ETriggerEvent::Canceled, this, &APlayerInputController::OnActionCanceled);
			break;

		default:
			// ETriggerEvent는 확장될 수 있으니 방어
			UE_LOG(LogPlayerInputController, Warning, TEXT("Unhandled ETriggerEvent value for Action '%s'."), *GetNameSafe(Action));
			break;
		}
	}

	
}

void APlayerInputController::OnActionStarted(const FInputActionInstance& Instance)
{
	DispatchSignal(ETriggerEvent::Started, Instance);
}

void APlayerInputController::OnActionOngoing(const FInputActionInstance& Instance)
{
	DispatchSignal(ETriggerEvent::Ongoing, Instance);
}

void APlayerInputController::OnActionTriggered(const FInputActionInstance& Instance)
{
	DispatchSignal(ETriggerEvent::Triggered, Instance);
}

void APlayerInputController::OnActionCompleted(const FInputActionInstance& Instance)
{
	DispatchSignal(ETriggerEvent::Completed, Instance);
}

void APlayerInputController::OnActionCanceled(const FInputActionInstance& Instance)
{
	DispatchSignal(ETriggerEvent::Canceled, Instance);
}

void APlayerInputController::DispatchSignal(ETriggerEvent Event, const FInputActionInstance& Instance)
{
	if (!InputSignalConfig)
	{
		return;
	}

	const UInputAction* SourceAction = Instance.GetSourceAction();
	if (!SourceAction)
	{
		return;
	}

	// (Action, TriggerEvent)로 Config 조회
	FActionSignal Key;
	Key.Action = SourceAction;
	Key.TriggerEvent = Event;

	const EInputSignalType* Found = InputSignalConfig->Signals.Find(Key);
	if (!Found)
	{
		// 바인딩은 되어 있는데, Config에 타입이 없다면(혹은 Key mismatch) 여기로 옴
		// 보통은 발생하면 안 되지만, 디버깅에 유용
		UE_LOG(LogPlayerInputController, VeryVerbose, TEXT("No signal mapping for Action '%s' Event '%d'."),
			*GetNameSafe(SourceAction), static_cast<int32>(Event));
		return;
	}

	HandleSignal(*Found, Instance);
}

void APlayerInputController::HandleSignal(EInputSignalType SignalType, const FInputActionInstance& Instance)
{
	// 여기서부터가 “우리 시스템” 라우팅 지점입니다.
	// 예시로만 남깁니다. (MoveHandler/SkillHandler/AttackHandler 등으로 전달)

	// const FInputActionValue& Value = Instance.GetValue();
	// switch (SignalType)
	// {
	// case EInputSignalType::Move:
	//     Router->RequestMove(Value.Get<FVector2D>());
	//     break;
	// case EInputSignalType::Attack:
	//     Router->RequestAttack();
	//     break;
	// case EInputSignalType::Skill1:
	//     Router->RequestSkill(ESkillDesignator::Skill1);
	//     break;
	// default:
	//     break;
	// }
}