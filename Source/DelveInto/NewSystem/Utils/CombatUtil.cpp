// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatUtil.h"

#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NewSystem/Entities/DelveEnemy.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"

bool UCombatUtil::ApplyDamageSphere(const UObject* WorldContextObject, AActor* Instigator, float Damage, float Radius, FVector OriginOffset, AActor* DamageCauser)
{
    bool bHit = false;

    if (!WorldContextObject || !Instigator) return bHit;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return bHit;

    // 시전자 정보 캐싱
    FVector Start = Instigator->GetActorLocation() + 
                    (Instigator->GetActorForwardVector() * OriginOffset.X) + 
                    (Instigator->GetActorRightVector() * OriginOffset.Y) + 
                    (Instigator->GetActorUpVector() * OriginOffset.Z);
    
    // 디버그 드로우
    DrawDebugSphere(World, Start, Radius, 12, FColor::Red, false, 1.0f);

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Instigator); // 시전자 본인 무시

    UKismetSystemLibrary::SphereOverlapActors(
        WorldContextObject, Start, Radius, ObjectTypes, nullptr, ActorsToIgnore, OverlappedActors
    );

    // DamageCauser가 없으면 Instigator를 Causer로 사용
    AActor* ActualCauser = DamageCauser ? DamageCauser : Instigator;
    AController* InstigatorController = Instigator->GetInstigatorController();

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor)
        {
            FDamageEvent DamageEvent;
            HitActor->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
            UE_LOG(LogTemp, Log, TEXT("Sphere Hit: %s, Damage: %f"), *HitActor->GetName(), Damage);
            bHit = true;
        }
    }

    return bHit;
}

bool UCombatUtil::ApplyDamageFanShape(const UObject* WorldContextObject, AActor* Instigator, float Damage, float Radius, float HalfAngle, float HalfHeight, AActor* DamageCauser)
{
    bool bHit = false;

    if (!WorldContextObject || !Instigator) return bHit;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return bHit;

    FVector Origin = Instigator->GetActorLocation();
    FVector Forward = Instigator->GetActorForwardVector();

    // 디버그 드로우
    DrawDebugCylinder(World, Origin - FVector(0,0,HalfHeight), Origin + FVector(0,0,HalfHeight), Radius, 12, FColor::Silver, false, 1.0f);
    FVector LeftDir = Forward.RotateAngleAxis(-HalfAngle, FVector::UpVector);
    FVector RightDir = Forward.RotateAngleAxis(HalfAngle, FVector::UpVector);
    DrawDebugLine(World, Origin, Origin + (LeftDir * Radius), FColor::Red, false, 1.0f);
    DrawDebugLine(World, Origin, Origin + (RightDir * Radius), FColor::Red, false, 1.0f);

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Instigator);

    UKismetSystemLibrary::CapsuleOverlapActors(
        WorldContextObject, Origin, Radius, HalfHeight, ObjectTypes, nullptr, ActorsToIgnore, OverlappedActors
    );

    AActor* ActualCauser = DamageCauser ? DamageCauser : Instigator;
    AController* InstigatorController = Instigator->GetInstigatorController();

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor)
        {
            FVector ToTarget = HitActor->GetActorLocation() - Origin;
            ToTarget.Z = 0.0f; // 높이 차이 제거
            
            FVector FlatForward = Forward;
            FlatForward.Z = 0.0f;

            ToTarget.Normalize();
            FlatForward.Normalize();

            float DotValue = FVector::DotProduct(FlatForward, ToTarget);
            float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(DotValue));

            if (AngleDegree <= HalfAngle)
            {
                FDamageEvent DamageEvent;
                HitActor->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
                UE_LOG(LogTemp, Log, TEXT("Fan Hit: %s (Angle: %f)"), *HitActor->GetName(), AngleDegree);
                bHit = true;
            }
        }
    }

    return bHit;
}

bool UCombatUtil::ApplyDamageSphericalCone(const UObject* WorldContextObject, AActor* Instigator, float Damage, float Radius, float HalfAngle, AActor* DamageCauser)
{
    bool bHit = false;
    
    if (!WorldContextObject || !Instigator) return bHit;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return bHit;

    FVector Origin;
    FVector Forward;

    // Instigator가 Character라면 카메라 위치 우선 사용
    if (ACharacter* OwnerChar = Cast<ACharacter>(Instigator))
    {
        if (UCameraComponent* Cam = OwnerChar->FindComponentByClass<UCameraComponent>())
        {
            Origin = Cam->GetComponentLocation();
            Forward = Cam->GetForwardVector();
        }
        else
        {
            Origin = OwnerChar->GetPawnViewLocation();
            Forward = OwnerChar->GetActorForwardVector();
        }
    }
    else
    {
        Origin = Instigator->GetActorLocation();
        Forward = Instigator->GetActorForwardVector();
    }

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Instigator);

    UKismetSystemLibrary::SphereOverlapActors(
        WorldContextObject, Origin, Radius, ObjectTypes, nullptr, ActorsToIgnore, OverlappedActors
    );

    AActor* ActualCauser = DamageCauser ? DamageCauser : Instigator;
    AController* InstigatorController = Instigator->GetInstigatorController();

    DrawDebugCone(
        World,
        Origin,
        Forward,
        Radius,
        FMath::DegreesToRadians(HalfAngle), // 각도
        FMath::DegreesToRadians(HalfAngle),
        60,
        FColor::Orange, // 주황색으로 표시
        false,
        3.0f
    );

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor)
        {
            FVector ToTarget = HitActor->GetActorLocation() - Origin;
            
            if (ToTarget.SizeSquared() > Radius * Radius) continue;

            ToTarget.Normalize();

            float DotValue = FVector::DotProduct(Forward, ToTarget);
            float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(DotValue));

            if (AngleDegree <= HalfAngle)
            {
                FDamageEvent DamageEvent;
                HitActor->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
                UE_LOG(LogTemp, Log, TEXT("3D Cone Hit: %s (Angle: %f)"), *HitActor->GetName(), AngleDegree);
                bHit = true;
            }
        }
    }

    return bHit;
}


bool UCombatUtil::IsEnemy(AActor* ActorA, AActor* ActorB)
{
    if (!ActorA || !ActorB || ActorA == ActorB) return false;

    bool bIsAPlayer = ActorA->IsA(ADelveCharacter::StaticClass());
    bool bIsBPlayer = ActorB->IsA(ADelveCharacter::StaticClass());

    bool bIsAEnemy = ActorA->IsA(ADelveEnemy::StaticClass());
    bool bIsBEnemy = ActorB->IsA(ADelveEnemy::StaticClass());

    // 한쪽이 플레이어고 다른 쪽이 적이면 true
    return (bIsAPlayer && bIsBEnemy) || (bIsAEnemy && bIsBPlayer);
}

bool UCombatUtil::IsFriendly(AActor* ActorA, AActor* ActorB)
{
    if (!ActorA || !ActorB) return false;
    if (ActorA == ActorB) return true; // 자기 자신은 아군

    bool bIsAPlayer = ActorA->IsA(ADelveCharacter::StaticClass());
    bool bIsBPlayer = ActorB->IsA(ADelveCharacter::StaticClass());

    bool bIsAEnemy = ActorA->IsA(ADelveEnemy::StaticClass());
    bool bIsBEnemy = ActorB->IsA(ADelveEnemy::StaticClass());

    // 둘 다 플레이어거나, 둘 다 적이면 true
    return (bIsAPlayer && bIsBPlayer) || (bIsAEnemy && bIsBEnemy);
}

bool UCombatUtil::ApplyDamageIfEnemy(const UObject* WorldContextObject, AActor* Instigator, AActor* Target, float Damage, AActor* DamageCauser)
{
    if (IsEnemy(Instigator, Target))
    {
        AActor* ActualCauser = DamageCauser ? DamageCauser : Instigator;
        AController* InstigatorController = Instigator ? Instigator->GetInstigatorController() : nullptr;
        
        FDamageEvent DamageEvent;
        Target->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
        return true;
    }
    return false;
}