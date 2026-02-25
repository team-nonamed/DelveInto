// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatUtil.h"

#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NewSystem/Entities/DelveEnemy.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Skills/SkillBase.h"

bool UCombatUtil::ApplyDamageSphere(const UObject* WorldContextObject, AActor* Instigator, float Damage, float Radius, FVector OriginOffset, AActor* DamageCauser)
{
    bool bHit = false;

    if (!WorldContextObject || !Instigator) return bHit;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return bHit;

    FVector Start = Instigator->GetActorLocation() + 
                    (Instigator->GetActorForwardVector() * OriginOffset.X) + 
                    (Instigator->GetActorRightVector() * OriginOffset.Y) + 
                    (Instigator->GetActorUpVector() * OriginOffset.Z);
#if WITH_EDITOR    
    DrawDebugSphere(World, Start, Radius, 12, FColor::Red, false, 1.0f);
#endif
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Instigator);

    UKismetSystemLibrary::SphereOverlapActors(
        WorldContextObject, Start, Radius, ObjectTypes, nullptr, ActorsToIgnore, OverlappedActors
    );

    AActor* ActualCauser = DamageCauser ? DamageCauser : Instigator;
    AController* InstigatorController = Instigator->GetInstigatorController();

    // [수정] 호출자가 스킬인지 확인합니다.
    USkillBase* CallerSkill = Cast<USkillBase>(const_cast<UObject*>(WorldContextObject));

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor)
        {
            if (CallerSkill)
            {
                // 플레이어 스킬이 호출한 경우: 퍽 연산을 위해 CombatHandler로 넘김
                CallerSkill->OnSkillHit.Broadcast(CallerSkill, HitActor, Damage);
            }
            else
            {
                // 몬스터 등이 호출한 경우: 즉시 데미지 적용
                FDamageEvent DamageEvent;
                HitActor->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
            }
            
            UE_LOG(LogTemp, Log, TEXT("Sphere Hit: %s, BaseDamage: %f"), *HitActor->GetName(), Damage);
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
#if WITH_EDITOR
    DrawDebugCylinder(World, Origin - FVector(0,0,HalfHeight), Origin + FVector(0,0,HalfHeight), Radius, 12, FColor::Silver, false, 1.0f);
#endif
    FVector LeftDir = Forward.RotateAngleAxis(-HalfAngle, FVector::UpVector);
    FVector RightDir = Forward.RotateAngleAxis(HalfAngle, FVector::UpVector);
#if WITH_EDITOR
    DrawDebugLine(World, Origin, Origin + (LeftDir * Radius), FColor::Red, false, 1.0f);
    DrawDebugLine(World, Origin, Origin + (RightDir * Radius), FColor::Red, false, 1.0f);
#endif
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

    // [수정] 호출자가 스킬인지 확인
    USkillBase* CallerSkill = Cast<USkillBase>(const_cast<UObject*>(WorldContextObject));

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor)
        {
            FVector ToTarget = HitActor->GetActorLocation() - Origin;
            ToTarget.Z = 0.0f; 
            
            FVector FlatForward = Forward;
            FlatForward.Z = 0.0f;

            ToTarget.Normalize();
            FlatForward.Normalize();

            float DotValue = FVector::DotProduct(FlatForward, ToTarget);
            float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(DotValue));

            if (AngleDegree <= HalfAngle)
            {
                if (CallerSkill)
                {
                    CallerSkill->OnSkillHit.Broadcast(CallerSkill, HitActor, Damage);
                }
                else
                {
                    FDamageEvent DamageEvent;
                    HitActor->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
                }

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

#if WITH_EDITOR
    DrawDebugCone(
        World, Origin, Forward, Radius,
        FMath::DegreesToRadians(HalfAngle), FMath::DegreesToRadians(HalfAngle),
        60, FColor::Orange, false, 3.0f
    );
#endif

    // [수정] 호출자가 스킬인지 확인
    USkillBase* CallerSkill = Cast<USkillBase>(const_cast<UObject*>(WorldContextObject));

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
                if (CallerSkill)
                {
                    // 델리게이트를 통해 CombatHandler로 전달 (퍽 연산 진행)
                    CallerSkill->OnSkillHit.Broadcast(CallerSkill, HitActor, Damage);
                }
                else
                {
                    // 몬스터의 공격 등은 다이렉트로 데미지 전달
                    FDamageEvent DamageEvent;
                    HitActor->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
                }

                UE_LOG(LogTemp, Warning, TEXT("3D Cone Hit: %s (Angle: %f)"), *HitActor->GetName(), AngleDegree);
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

    return (bIsAPlayer && bIsBEnemy) || (bIsAEnemy && bIsBPlayer);
}

bool UCombatUtil::IsFriendly(AActor* ActorA, AActor* ActorB)
{
    if (!ActorA || !ActorB) return false;
    if (ActorA == ActorB) return true;

    bool bIsAPlayer = ActorA->IsA(ADelveCharacter::StaticClass());
    bool bIsBPlayer = ActorB->IsA(ADelveCharacter::StaticClass());

    bool bIsAEnemy = ActorA->IsA(ADelveEnemy::StaticClass());
    bool bIsBEnemy = ActorB->IsA(ADelveEnemy::StaticClass());

    return (bIsAPlayer && bIsBPlayer) || (bIsAEnemy && bIsBEnemy);
}

bool UCombatUtil::ApplyDamageIfEnemy(const UObject* WorldContextObject, AActor* Instigator, AActor* Target, float Damage, AActor* DamageCauser)
{
    if (IsEnemy(Instigator, Target))
    {
        AActor* ActualCauser = DamageCauser ? DamageCauser : Instigator;
        AController* InstigatorController = Instigator ? Instigator->GetInstigatorController() : nullptr;
        
        USkillBase* CallerSkill = Cast<USkillBase>(const_cast<UObject*>(WorldContextObject));
        if (CallerSkill)
        {
            CallerSkill->OnSkillHit.Broadcast(CallerSkill, Target, Damage);
        }
        else
        {
            FDamageEvent DamageEvent;
            Target->TakeDamage(Damage, DamageEvent, InstigatorController, ActualCauser);
        }
        return true;
    }
    return false;
}