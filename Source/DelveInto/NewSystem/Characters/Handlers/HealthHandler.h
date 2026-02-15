// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthHandler.generated.h"

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamagedDelegate, float, MaxHealth, float, CurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealedDelegate, float, MaxHealth, float, CurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathDelegate, class ACharacter*, DeadCharacter);

UCLASS(ClassGroup=(Handler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UHealthHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthHandler();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

#pragma region Properties
public:
	/** 최대 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	// [수정] UPROPERTY 뒤에 세미콜론(;) 제거
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsDead = false;

	// [이벤트] BlueprintAssignable 필수
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamagedDelegate OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathDelegate OnDeath;
	
#pragma endregion 

#pragma region Methods
public: 
	// 데미지 처리 함수
	float ApplyDamage(float DamageAmount);

#pragma endregion
};