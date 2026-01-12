// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractAttackHandler.h"

#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Items/ItemDefinition.h"


#include "Messages/InnerResult.h"
#include "Skills/Instances/SkillInstance.h"


// Sets default values for this component's properties
UAbstractAttackHandler::UAbstractAttackHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbstractAttackHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAbstractAttackHandler::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FHurtResult UAbstractAttackHandler::IssueAttack(
	ESkillDesignator Designator)
{
	const USkillInstance* Skill = Skills->GetSkill(Designator);
	const UItemInstance* Weapon = Inventory->GetActivatedWeapon();

	if (!Skill)
	{
		return FHurtResult(EResultType::Invalid);
	}

	if (!Weapon)
	{
		return FHurtResult(EResultType::Invalid);
	}

	TSet<EWeaponCategory> SkillWeaponCategory = Skill->GetSkillData()->AllowedWeaponCategory;
	EWeaponCategory WeaponCategory = Weapon->GetWeaponCategory();

	if (!SkillWeaponCategory.Contains(WeaponCategory))
	{
		return FHurtResult(EResultType::InvalidWeapon);
	}
	
	Weapon->DoAttack(Skill, Modifiers);

	// 공격자가 들고 있는 무기와 사용한 스킬의 피해량 증감 계산
	float BaseDamage = Weapon->GetBaseDamage();
	BaseDamage *= 1 + Skill->GetDamageMultiplierAdditive();
	BaseDamage += Skill->GetDamageAdditive();
	
	if (BaseDamage <= DBL_EPSILON)
	{
		return FHurtResult(EResultType::Impossible);
	}

	float CurrentAdditive = 0.0f;
	float CurrentMultiplier = 1.0f;

	FInnerResult InnerResult = FInnerResult();
	
	for (TScriptInterface<IDamageModifier>& Modifier : Modifiers)
	{
		if (!Modifier)
		{
			continue;
		}

		CurrentAdditive += Modifier->GetDamageAdditive();
		CurrentMultiplier += Modifier->GetDamageMultiplierAdditive();

		InnerResult.ApplyModifier(Modifier->GetCancelled(), Modifier->GetPriority());
	}

	if (InnerResult.IsCancelled())
	{
		return FHurtResult(EResultType::Cancelled);
	}

	float CurrentDamage = BaseDamage * CurrentMultiplier + CurrentAdditive;
	
	if (CurrentDamage <= DBL_EPSILON)
	{
		return FHurtResult(EResultType::Impossible);
	}

	const FHurtRequest Request = FHurtRequest(Instigator, Receiver, CurrentDamage, IsCritical);

	return Receiver->ReceiveHurt(Request);
}

float UAbstractAttackHandler::GetWeaponDamage() const
{
	return Weapon->GetBaseDamage();
}

float UAbstractAttackHandler::GetCurrentAttackStat() const
{
	return Attack;
}

float UAbstractAttackHandler::GetBaseAttackDamage() const
{
	return Weapon->GetBaseDamage() + Attack;
}

TArray<TScriptInterface<IHurtReceiver>> UAbstractAttackHandler::FindActorsInCone(
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
	TArray<TScriptInterface<IHurtReceiver>> Result;

	if (!World || Radius <= 0.f || HalfAngleDeg <= 0.f)
	{
		return Result;
	}

	// 1) Sphere overlap로 후보 수집
	TArray<FOverlapResult> Overlaps;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindActorsInCone), /*bTraceComplex=*/false);
	for (AActor* Ignored : ActorsToIgnore)
	{
		if (Ignored)
		{
			QueryParams.AddIgnoredActor(Ignored);
		}
	}

	const bool bAny = World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);

	if (!bAny)
	{
		return Result;
	}

	// 2) 각도 임계값 준비
	FVector Fwd = Forward;
	if (bIgnoreZ)
	{
		Fwd.Z = 0.f;
	}
	if (!Fwd.Normalize())
	{
		return Result;
	}

	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(HalfAngleDeg));
	const float RadiusSq = Radius * Radius;

	// 거리 정렬/최대 N개를 위해 (Iface, DistSq)로 임시 저장
	TArray<TPair<TScriptInterface<IHurtReceiver>, float>> Candidates;
	Candidates.Reserve(Overlaps.Num());

	for (const FOverlapResult& O : Overlaps)
	{
		AActor* CandidateActor = O.GetActor();
		if (!CandidateActor)
		{
			continue;
		}

		// 3) IHurtReceiver 구현 여부 필터
		if (!CandidateActor->GetClass()->ImplementsInterface(UHurtReceiver::StaticClass()))
		{
			continue;
		}

		// 4) 거리/각도 필터
		FVector To = CandidateActor->GetActorLocation() - Origin;
		if (bIgnoreZ)
		{
			To.Z = 0.f;
		}

		const float DistSq = To.SizeSquared();
		if (DistSq > RadiusSq || DistSq <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		const FVector Dir = To.GetSafeNormal();
		const float Dot = FVector::DotProduct(Fwd, Dir);
		if (Dot < CosThreshold)
		{
			continue;
		}

		// 5) LOS(옵션)
		if (bRequireLineOfSight)
		{
			FHitResult Hit;
			const FVector Start = Origin;
			const FVector End = CandidateActor->GetActorLocation();

			const bool bBlocked = World->LineTraceSingleByChannel(
				Hit,
				Start,
				End,
				TraceChannelForLOS,
				QueryParams
			);

			if (bBlocked && Hit.GetActor() != CandidateActor)
			{
				continue;
			}
		}

		// 6) TScriptInterface 구성
		TScriptInterface<IHurtReceiver> ReceiverIface;
		ReceiverIface.SetObject(CandidateActor);
		ReceiverIface.SetInterface(Cast<IHurtReceiver>(CandidateActor)); // ImplementsInterface면 Cast 성공해야 정상

		if (!ReceiverIface) // 방어
		{
			continue;
		}

		Candidates.Add({ ReceiverIface, DistSq });
	}

	if (Candidates.Num() == 0)
	{
		return Result;
	}

	// 7) 가까운 순 정렬 + 최대 타겟 제한
	Candidates.Sort([](const auto& A, const auto& B)
	{
		return A.Value < B.Value;
	});

	const int32 Take = (MaxTargets > 0) ? FMath::Min(MaxTargets, Candidates.Num()) : Candidates.Num();
	Result.Reserve(Take);

	for (int32 i = 0; i < Take; ++i)
	{
		Result.Add(Candidates[i].Key);
	}

	return Result;
}