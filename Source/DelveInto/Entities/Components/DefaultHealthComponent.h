// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "Components/ActorComponent.h"
// #include "Entities/Contexts/HealthChangeContext.h"
// #include "Entities/Interactions/HealthInstigation.h"
// #include "HealthComponent.generated.h"
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
// 	FOnHurt,
// 	FHealthChangeContext, Context
// );
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
// 	FOnRecover,
// 	FHealthChangeContext, Context
// );
//
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
// 	FOnDeath,
// 	FHealthChangeContext, LastHealthChangeContext
// );
//
// /**
//  * 체력을 관리하는 Component
//  * - 체력 상태를 보관하고
//  * - 피해/회복/공격 적용을 처리하며
//  * - 체력 변화/사망 이벤트를 브로드캐스트한다.
//  */
// UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
// class DELVEINTO_API UHealthComponent : public UActorComponent
// {
// 	GENERATED_BODY()
//
//
// protected:
// #pragma region Attributes
// 	/**
// 	 * 최대 체력
// 	 */
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
// 	float MaxHealth = 100.0f;
//
// 	/**
// 	 * 처음 Initialize될 때 최대 체력으로 초기화 할지에 대한 여부
// 	 */
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
// 	bool bInitWithMaxHealth = true;
//
// 	/**
// 	 * 현재 체력
// 	 */
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
// 	float CurrentHealth = 100.0f;
//
// 	/**
// 	 * 사망 여부 (OnDeath 중복 방지)
// 	 */
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
// 	bool bIsDead = false;
// #pragma endregion
//
// public:
// #pragma region Actions
// 	UPROPERTY(BlueprintAssignable, Category="Health")
// 	FOnHurt OnHurtStart;
//
// 	UPROPERTY(BlueprintAssignable, Category="Health")
// 	FOnHurt OnHurtEnd;
//
// 	UPROPERTY(BlueprintAssignable, Category="Health")
// 	FOnRecover OnRecoverStart;
//
// 	UPROPERTY(BlueprintAssignable, Category="Health")
// 	FOnRecover OnRecoverEnd;
//
// 	UPROPERTY(BlueprintAssignable, Category="Health")
// 	FOnDeath OnDeathStart;
//
// 	UPROPERTY(BlueprintAssignable, Category="Health")
// 	FOnDeath OnDeathEnd;
// #pragma endregion
// 	/**
// 	 * Constructor
// 	 */
// 	UHealthComponent();
//
// 	UFUNCTION(BlueprintCallable, Category="Health")
// 	bool ApplyInstigation(const FHealthInstigation& Instigation);
//
// 	UFUNCTION(BlueprintPure, Category="Health")
// 	bool IsDead() const { return bIsDead; }
//
// 	UFUNCTION(BlueprintPure, Category="Health")
// 	float GetCurrentHealth() const { return CurrentHealth; }
//
// 	UFUNCTION(BlueprintPure, Category="Health")
// 	float GetMaxHealth() const { return MaxHealth; }
//
// protected:
// 	// Called when the game starts
// 	virtual void BeginPlay() override;
// 	
// 	virtual void ApplyHurt(FHealthChangeContext& Context);
//
// 	virtual void ApplyRecover(FHealthChangeContext& Context);
// };

#pragma once

#include "CoreMinimal.h"
#include "Entities/Interfaces/HurtModifier.h"
#include "Entities/Messages/Results/HurtResult.h"
#include "Entities/Messages/Requests/HurtRequest.h"
#include "Components/ActorComponent.h"

#include "DefaultHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnHurt,
	const FHurtResult&, Result
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDeath,
	const FHurtResult&, LastHurtResult
);

UCLASS(ClassGroup=(Health), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UDefaultHealthComponent: public UActorComponent
{
	GENERATED_BODY()

protected:
#pragma region Attributes
#pragma region Health Related
	/**
	 * 최대 체력
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float MaxHealth = 100.0f;

	/**
	 * 처음 Initialize될 때 최대 체력으로 초기화 할지에 대한 여부
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health | Config")
	bool bInitWithMaxHealth = true;

	/**
	 * 현재 체력
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
	float CurrentHealth = 100.0f;

	/**
	 * 사망 여부 (OnDeath 중복 방지)
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
	bool bIsDead = false;
#pragma endregion

#pragma region Modifiers
	UPROPERTY(Transient)
	TArray<TScriptInterface<IHurtModifier>> Modifiers;
#pragma endregion

#pragma region Delegates
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHurt OnHurt;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDeath OnDeath;
#pragma endregion
#pragma endregion

public:
#pragma region Methods
#pragma region Handlers
	UFUNCTION(BlueprintCallable, Category="Health")
	FHurtResult ApplyHurtRequest(const FHurtRequest& Request);

	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category="Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHealth() const { return MaxHealth; }
	
#pragma endregion

#pragma region Register
	UFUNCTION(BlueprintCallable, Category="Health | Events")
	bool BindOnHurt(UObject* Listener, FName FunctionName);

	UFUNCTION(BlueprintCallable, Category="Health | Events")
	bool UnbindOnHurt(UObject* Listener, FName FunctionName);

	UFUNCTION(BlueprintCallable, Category="Health | Events")
	bool UnbindAllOnHurt(UObject* Listener);

	
	UFUNCTION(BlueprintCallable, Category="Health|Events")
	bool BindOnDeath(UObject* Listener, FName FunctionName);

	UFUNCTION(BlueprintCallable, Category="Health|Events")
	bool UnbindOnDeath(UObject* Listener, FName FunctionName);

	UFUNCTION(BlueprintCallable, Category="Health|Events")
	bool UnbindAllOnDeath(UObject* Listener);
	
	UFUNCTION(BlueprintCallable, Category="Health | Modifiers")
	bool RegisterHurtModifier(TScriptInterface<IHurtModifier> Modifier);

	UFUNCTION(BlueprintCallable, Category="Health | Modifiers")
	bool UnregisterHurtModifier(TScriptInterface<IHurtModifier> Modifier);
#pragma endregion
#pragma endregion
};