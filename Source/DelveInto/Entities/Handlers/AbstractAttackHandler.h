// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Attacks/AttackIssuer.h"
#include "Interfaces/Damages/DamageModifier.h"
#include "Interfaces/Damages/DamageProvider.h"
#include "Interfaces/Skills/SkillProvider.h"
#include "Interfaces/Stats/AttackStatProvider.h"
#include "AbstractAttackHandler.generated.h"


UCLASS(ClassGroup=(AttackHandler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UAbstractAttackHandler : public UActorComponent, public IAttackIssuer, public IAttackStatProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbstractAttackHandler();

	TScriptInterface<IDamageProvider> Weapon;

	TWeakInterfacePtr<ISkillProvider> Skills;

	TArray<TScriptInterface<IDamageModifier>> Modifiers;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Attack;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual FHurtResult IssueAttack(
		ESkillDesignator Designator) override;

	virtual float GetWeaponDamage() const override;
	virtual float GetCurrentAttackStat() const override;
	virtual float GetBaseAttackDamage() const override;

	static TArray<TScriptInterface<IHurtReceiver>> FindActorsInCone(
		UWorld* World,
		const FVector& Origin,
		const FVector& Forward,
		float Radius,
		float HalfAngleDeg,
		const FCollisionObjectQueryParams& ObjectQueryParams,
		ECollisionChannel TraceChannelForLOS,
		const TArray<AActor*>& ActorsToIgnore,
		bool bRequireLineOfSight,
		bool bIgnoreZ,
		int32 MaxTargets = 0
	);
	
};
