// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractEntity.h"


// Sets default values
AAbstractEntity::AAbstractEntity()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAbstractEntity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAbstractEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FHurtResult AAbstractEntity::ReceiveHurt(const FHurtRequest& Request)
{
	checkf(false, TEXT("ReceiveHurt must be implemented in derived class."));
	return FHurtResult();
}