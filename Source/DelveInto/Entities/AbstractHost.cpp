// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractHost.h"

#include "Handlers/Healths/HealthHandler.h"


// Sets default values
AAbstractHost::AAbstractHost()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAbstractHost::SetHealthHandler(UAbstractHealthHandler* Handler)
{

	if (HealthHandler == nullptr)
	{
		HealthHandler = Handler;
	}

}

void AAbstractHost::SetAttackHandler(UAbstractAttackHandler* Handler)
{
	if (AttackHandler == nullptr)
	{
		AttackHandler = Handler;
	}
}

void AAbstractHost::SetSkillHandler(UAbstractSkillHandler* Handler)
{
	if (SkillHandler == nullptr)
	{
		SkillHandler = Handler;
	}
}

// Called when the game starts or when spawned
void AAbstractHost::BeginPlay()
{
	Super::BeginPlay();

	if (HealthHandler == nullptr)
	{
		HealthHandler = FindComponentByClass<UAbstractHealthHandler>();
	}

	if (SkillHandler == nullptr)
	{
		SkillHandler = FindComponentByClass<UAbstractSkillHandler>();
	}

	if (AttackHandler == nullptr)
	{
		AttackHandler = FindComponentByClass<UAbstractAttackHandler>();
	}

	if (HealthHandler != nullptr)
	{
		UE_LOG(LogActor, Log, TEXT("Health handler %s"), *HealthHandler->GetName());
	}

	if (SkillHandler != nullptr)
		UE_LOG(LogActor, Log, TEXT("Skill handler %s"), *SkillHandler->GetName());

	if (AttackHandler != nullptr)
		UE_LOG(LogActor, Log, TEXT("Attack %s"), *AttackHandler->GetName());
}

// Called every frame
void AAbstractHost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FHurtResult AAbstractHost::ReceiveHurt(const FHurtRequest& Request)
{
	if (HealthHandler == nullptr)
	{
		return FHurtResult(EResultType::Invalid);
	}

	return HealthHandler->HandleHurt(Request);
}

FHurtResult AAbstractHost::InstigateAttack(ESkillDesignator Designator)
{
}