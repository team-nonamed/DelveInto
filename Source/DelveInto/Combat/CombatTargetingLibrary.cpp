#include "Combat/CombatTargetingLibrary.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

static bool HasLOS(UWorld* World, const FVector& From, AActor* ToActor, ECollisionChannel Channel, const TArray<AActor*>& Ignore)
{
	if (!World || !ToActor) return false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(LOS), false);
	Params.AddIgnoredActors(Ignore);

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(
		Hit,
		From,
		ToActor->GetActorLocation(),
		Channel,
		Params
	);

	// 막힌 게 없거나, 맞은 대상이 ToActor면 LOS OK
	if (!bHit) return true;
	return Hit.GetActor() == ToActor;
}

TArray<AActor*> UCombatTargetingLibrary::FindActorsInCircle(
	UWorld* World,
	const FVector& Origin,
	float Radius,
	const FCollisionObjectQueryParams& ObjectQueryParams,
	ECollisionChannel TraceChannelForLOS,
	const TArray<AActor*>& ActorsToIgnore,
	bool bRequireLineOfSight,
	int32 MaxTargets)
{
	TArray<AActor*> Out;
	if (!World) return Out;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FindActorsInCircle), false);
	Params.AddIgnoredActors(ActorsToIgnore);

	const bool bOk = World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (!bOk) return Out;

	for (const FOverlapResult& R : Overlaps)
	{
		AActor* A = R.GetActor();
		if (!A) continue;

		if (bRequireLineOfSight && !HasLOS(World, Origin, A, TraceChannelForLOS, ActorsToIgnore))
		{
			continue;
		}

		Out.Add(A);
		if (MaxTargets > 0 && Out.Num() >= MaxTargets)
		{
			break;
		}
	}

	return Out;
}

TArray<AActor*> UCombatTargetingLibrary::FindActorsInCone(
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
	int32 MaxTargets)
{
	TArray<AActor*> Out;
	if (!World) return Out;

	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(HalfAngleDeg));
	const FVector Fwd = Forward.GetSafeNormal();

	// 1) 원형 후보 수집
	TArray<AActor*> Candidates = FindActorsInCircle(
		World, Origin, Radius, ObjectQueryParams, TraceChannelForLOS,
		ActorsToIgnore, false, 0
	);

	// 2) 각도 필터 + (선택) LOS
	for (AActor* A : Candidates)
	{
		if (!A) continue;

		FVector Dir = (A->GetActorLocation() - Origin);
		if (bIgnoreZ)
		{
			Dir.Z = 0.f;
		}
		Dir = Dir.GetSafeNormal();

		const float Cos = FVector::DotProduct(Fwd, Dir);
		if (Cos < CosThreshold)
		{
			continue;
		}

		if (bRequireLineOfSight && !HasLOS(World, Origin, A, TraceChannelForLOS, ActorsToIgnore))
		{
			continue;
		}

		Out.Add(A);
		if (MaxTargets > 0 && Out.Num() >= MaxTargets)
		{
			break;
		}
	}

	return Out;
}
