// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DefaultPlayerController.h"


ADefaultPlayerController::ADefaultPlayerController()
{
	bShowMouseCursor       = false;
	bEnableClickEvents     = false;
	bEnableMouseOverEvents = false;
}

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// IMC 추가도 Blueprint에서 할 거면 여기서 아무것도 안 해도 됨
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// 여기에서 EnhancedInputComponent 캐스팅 + BindAction 안 해도 됨
}

// ===== BP에서 호출할 브로드캐스트 함수들 =====

void ADefaultPlayerController::EmitMove(FVector2D MoveAxis)
{
	OnMove.Broadcast(MoveAxis);
}

void ADefaultPlayerController::EmitLook(FVector2D LookAxis)
{
	OnLook.Broadcast(LookAxis);
}

void ADefaultPlayerController::EmitSprintStart()
{
	OnSprintStart.Broadcast();
}

void ADefaultPlayerController::EmitSprintEnd()
{
	OnSprintEnd.Broadcast();
}

void ADefaultPlayerController::EmitInteract()
{
	OnInteract.Broadcast();
}