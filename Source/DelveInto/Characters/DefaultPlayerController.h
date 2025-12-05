// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMove, FVector2D, MoveAxis);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLook, FVector2D, LookAxis);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UENUM(BlueprintType)
enum class EPlayerInputContext : uint8
{
	GamePlay,
	UIOnly,
	GameAndUI,
};

/**
 * 캐릭터의 입력을 처리하는 Class
 */
UCLASS()
class DELVEINTO_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Player의 현재 Input Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Mapping")
	TMap<EPlayerInputContext, UInputMappingContext*> InputMap;

	// 현재 Input Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Mapping")
	EPlayerInputContext CurrentInputMode = EPlayerInputContext::GamePlay;

	// Input Context를 전환하는 Method
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ApplyInputContext(EPlayerInputContext NewInputContext);

protected:
	void BeginPlay() override;
	
	// Component에서 Binding하리라 기대하는 이동 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Behaviour Events")
	FOnMove OnMove;

	// Component에서 Binding하리라 기대하는 Look 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Behaviour Events")
	FOnLook OnLook;

	// Component에서 Binding하리라 기대하는 달리기 시작 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Behaviour Events")
	FOnSprintStart OnSprintStart;

	// Component에서 Binding하리라 기대하는 달리기 종료 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Behaviour Events")
	FOnSprintEnd OnSprintEnd;
	
	// Component에서 Binding하리라 기대하는 상호작용 이벤트
	UPROPERTY(BlueprintAssignable, Category="Behaviour Events")
	FOnInteract OnInteract;

	
public:
	// BP에서 호출해서 델리게이트를 쏘는 함수들
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitMove(FVector2D MoveAxis);

	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitLook(FVector2D LookAxis);

	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitSprintStart();

	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitSprintEnd();

	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitInteract();

	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitOpenInventory();
};
