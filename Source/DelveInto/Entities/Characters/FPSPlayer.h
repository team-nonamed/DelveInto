// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "FPSPlayer.generated.h"

UCLASS()
class DELVEINTO_API AFPSPlayer : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "View", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FPSCamera;

public:
	/**
	 * AFPSPlayer Constructor
	 */
	AFPSPlayer();

protected:
	/**
	 * 게임이 시작하거나 Spawn될 때 호출되는 Methods
	 */
	virtual void BeginPlay() override;

	/**
	 * Input Controller의 OnMove를 받는 Callback
	 * @param DeltaMove 움직일 Move Vector 값
	 */
	UFUNCTION()
	void HandleMove(FVector2D DeltaMove);

	/**
	 * Input Controller의 OnLook을 받는 Callback
	 * @param DeltaLook 움직인 Look Vector 값
	 */
	UFUNCTION()
	void HandleLook(FVector2D DeltaLook);
};
