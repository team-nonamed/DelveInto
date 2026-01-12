// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Handlers/AbstractAttackHandler.h"
#include "Handlers/AbstractHealthHandler.h"
#include "Handlers/Healths/HealthHandler.h"
#include "Handlers/Skills/SkillHandler.h"
#include "Interfaces/Attacks/AttackInstigator.h"
#include "Interfaces/Hurts/HurtReceiver.h"
#include "AbstractHost.generated.h"

UCLASS(BlueprintType, Blueprintable)
/**
 * 
 */
class DELVEINTO_API AAbstractHost : public ACharacter, public IHurtReceiver, public IAttackInstigator
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, BlueprintSetter=SetHealthHandler, EditDefaultsOnly, Category="Handlers")
	TObjectPtr<UAbstractHealthHandler> HealthHandler = nullptr;

	/**
	 * 공격
	 */
	UPROPERTY(BlueprintReadWrite, BlueprintSetter=SetAttackHandler, EditDefaultsOnly, Category="Handlers")
	TObjectPtr<UAbstractAttackHandler> AttackHandler = nullptr;

	UPROPERTY(BlueprintReadWrite, BlueprintSetter=SetSkillHandler, EditDefaultsOnly, Category="Handlers")
	TObjectPtr<UAbstractSkillHandler> SkillHandler;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AAbstractHost();

	UFUNCTION(BlueprintSetter)
	void SetHealthHandler(UAbstractHealthHandler* Handler);

	UFUNCTION(BlueprintSetter)
	void SetAttackHandler(UAbstractAttackHandler* Handler);

	UFUNCTION(BlueprintSetter)
	void SetSkillHandler(UAbstractSkillHandler* Handler);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Components|Health")
	virtual FHurtResult ReceiveHurt(const FHurtRequest& Request) override;

	UFUNCTION(BlueprintCallable, Category="Components|Health")
	virtual FHurtResult InstigateAttack(ESkillDesignator Designator) override;
};
