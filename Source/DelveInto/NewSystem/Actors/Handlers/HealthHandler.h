// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathDelegate, class ACharacter*, DeadCharacter);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UHealthHandler : public UActorComponent
{
	GENERATED_BODY()

#pragma region Inner Events
public:
	UHealthHandler();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Properties
public:
	/**
	 * 이 Handler를 소유하고 있는 Character가 가질 수 있는 최대 체력
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsDead = false;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHPChangedDelegate OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnDeathDelegate OnDeath;
	
#pragma endregion 

#pragma region Methods
public: 
	float ApplyDamage(float DamageAmount);

	
#pragma endregion
};
