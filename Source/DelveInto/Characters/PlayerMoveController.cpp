// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerMoveController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerMoveController::APlayerMoveController()
	: InputMappingContext(nullptr),
	  MoveAction(nullptr),
	  LookAction(nullptr),
	  SprintAction(nullptr)
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void APlayerMoveController::BeginPlay()
{
	Super::BeginPlay();

	if (LoadingWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading widget class"));
		LoadingWidgetInstance = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
		if (LoadingWidgetInstance)
		{
			LoadingWidgetInstance->AddToViewport();
		}
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// 필요하면 기존 매핑 제거
			Subsystem->ClearAllMappings();

			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, /*Priority=*/0);
			}
		}

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}

void APlayerMoveController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Move: Axis2D (WASD)
        if (MoveAction)
        {
            EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerMoveController::OnMove);
            EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerMoveController::OnMove);
        }

        // Look: Axis2D (Mouse X/Y)
        if (LookAction)
        {
            EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerMoveController::OnLook);
        }

        // Sprint: Digital (Shift)
        if (SprintAction)
        {
            EIC->BindAction(SprintAction, ETriggerEvent::Started,   this, &APlayerMoveController::OnSprintStart);
            EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerMoveController::OnSprintEnd);
        }

        // 점프는 아예 바인딩하지 않음 → "점프 없는" 세팅
    }
}

// ====== 이동 처리 ======
void APlayerMoveController::OnMove(const FInputActionValue& Value)
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    const FVector2D MoveAxis = Value.Get<FVector2D>();
    if (MoveAxis.IsNearlyZero()) return;

    // 컨트롤러 yaw 기준으로 월드 방향 벡터 계산
    const FRotator ControlRot = GetControlRotation();
    const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

    const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    ControlledPawn->AddMovementInput(Forward, MoveAxis.Y); // W/S
    ControlledPawn->AddMovementInput(Right,   MoveAxis.X); // A/D
}

// ====== 시야 회전 ======
void APlayerMoveController::OnLook(const FInputActionValue& Value)
{
    const FVector2D LookAxis = Value.Get<FVector2D>();
    if (LookAxis.IsNearlyZero()) return;

    // PlayerController 기준 입력 → 카메라 pitch/yaw 로 전달됨
    AddYawInput(LookAxis.X);
    AddPitchInput(LookAxis.Y);
}

// ====== 스프린트 ======
void APlayerMoveController::OnSprintStart(const FInputActionValue& /*Value*/)
{
    SetCurrentSpeed(SprintSpeed);
}

void APlayerMoveController::OnSprintEnd(const FInputActionValue& /*Value*/)
{
    SetCurrentSpeed(WalkSpeed);
}

void APlayerMoveController::SetCurrentSpeed(float NewSpeed)
{
	if (GetCharacter())
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacter()->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = NewSpeed;
		}
	}
}