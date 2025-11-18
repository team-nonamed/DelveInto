// Fill out your copyright notice in the Description page of Project Settings.
// EnemyAIController.cpp

#include "DefaultEnemyAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet2/KismetDebugUtilities.h"

ADefaultEnemyAIController::ADefaultEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ADefaultEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTargetByTag();
	UpdateMovementTowardsTarget();
}

void ADefaultEnemyAIController::UpdateTargetByTag()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateTargetByTag: No pawn possessed"));
		CurrentTarget = nullptr;
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateTargetByTag: World is null"));
		CurrentTarget = nullptr;
		return;
	}

	const FVector Center = ControlledPawn->GetActorLocation();

	TArray<AActor*> OverlappedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(ControlledPawn);

	UKismetSystemLibrary::SphereOverlapActors(
		World,
		Center,
		SearchRadius,
		ObjectTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		OverlappedActors
	);

	float BestDistSq = TNumericLimits<float>::Max();
	ACharacter* BestTarget = nullptr;

	for (AActor* Actor : OverlappedActors)
	{
		if (!Actor) continue;

		if (!Actor->ActorHasTag(TargetActorTag))
		{
			continue;
		}

		ACharacter* TargetCharacter = Cast<ACharacter>(Actor);
		if (!TargetCharacter) continue;

		const float DistSq = FVector::DistSquared(
			TargetCharacter->GetActorLocation(),
			Center
		);

		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = TargetCharacter;
		}
	}

	if (BestTarget)
	{
		UE_LOG(LogTemp, Display, TEXT("Best Target: %s"), *BestTarget->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Best Target: None (No target in range)"));
	}

	CurrentTarget = BestTarget;
}

void ADefaultEnemyAIController::UpdateMovementTowardsTarget()
{
	if (!CurrentTarget)
	{
		// 타깃 없으면 멈춤
		StopMovement();
		//UE_LOG(LogTemp, Verbose, TEXT("No CurrentTarget, stopping movement"));
		return;
	}

	EPathFollowingRequestResult::Type Result =
		MoveToActor(CurrentTarget, 100.f, true);

	UE_LOG(LogTemp, Verbose,
		TEXT("MoveToActor(%s) Result = %d"),
		*CurrentTarget->GetName(),
		(int32)Result);
}