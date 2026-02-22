#include "DelveEnemy_Jumper.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// [Override] 근접 공격 대신 점프 수행
void ADelveEnemy_Jumper::ExecuteAttack()
{
    // 타겟이 유효하지 않으면 종료
    if (bIsDead || !CachedTarget) 
    {
        FinishAttack();
        return;
    }

    // UE_LOG(LogTemp, Warning, TEXT("Jumper: Execute Jump!"));

    bHasHitTargetInAir = false;

    // 1. 점프 방향 계산
    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = CachedTarget->GetActorLocation();
    FVector Direction = (TargetLoc - MyLoc);
    Direction.Z = 0.0f;
    Direction.Normalize();

    // 2. 캐릭터 날리기 (점프)
    UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    FVector LaunchVel = (Direction * 600.0f) + FVector(0.0f, 0.0f, 500.0f);
    LaunchCharacter(LaunchVel, true, true);

    // 3. 공중 모션 재생
    if (EnemyFlipbook && JumpAirFlipbook)
    {
        EnemyFlipbook->SetFlipbook(JumpAirFlipbook);
        EnemyFlipbook->SetLooping(true);
        EnemyFlipbook->Play();
    }

    // *주의* 부모처럼 Timer로 FinishAttack을 부르지 않음.
    // 점퍼는 '착지(Landed)' 했을 때가 끝이기 때문.
}

// 공중 충돌 (몸통 박치기)
void ADelveEnemy_Jumper::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    // 공격 중일 때만, 그리고 공중에서 한 번만
    if (!bIsAttacking || bHasHitTargetInAir || bIsDead) return;

    // 플레이어와 부딪혔는지 확인
    if (Other && Other == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
    {
        // UE_LOG(LogTemp, Warning, TEXT("Jumper: Hit Player in Air!"));
        bHasHitTargetInAir = true;

        // 데미지
        FDamageEvent DamageEvent;
        Other->TakeDamage(3.0f, DamageEvent, GetController(), this);

        // 타격 모션 (부모의 AttackFlipbook 활용)
        if (EnemyFlipbook && AttackFlipbook)
        {
            EnemyFlipbook->SetFlipbook(AttackFlipbook);
            EnemyFlipbook->SetLooping(false);
            EnemyFlipbook->PlayFromStart();
        }
    }
}

// 착지 처리
void ADelveEnemy_Jumper::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    // 공격 중에 착지했다면?
    if (bIsAttacking)
    {
        // 1. 착지 데미지 (충격파)
        ApplyLandingDamage();

        // 2. 연속 공격 판단
        AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        bool bShouldChainJump = false;

        if (Player && !bIsDead)
        {
            float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
            
            // 사거리 안이고 너무 가깝지 않으면 연속 점프
            if (Dist <= JumpAttackRange && Dist > 150.0f)
            {
                bShouldChainJump = true;
            }
        }

        if (bShouldChainJump)
        {
            // [연속 점프] 
            // UE_LOG(LogTemp, Warning, TEXT("Jumper: Chain Jump!"));
            
            if (Player)
            {
                // 방향 보정
                FVector Dir = Player->GetActorLocation() - GetActorLocation();
                Dir.Z = 0.0f;
                SetActorRotation(Dir.Rotation());
                
                // ***중요*** 다시 공격 시작을 위해 상태 강제 리셋
                bIsAttacking = false; 
                bCanAttack = true; 
                
                // 다시 부모의 차징 시퀀스 호출 (준비 -> 점프 반복)
                StartAttackSequence(Player);
            }
        }
        else
        {
            // 연속 공격 안 하면 정상 종료 (쿨타임 시작, Idle 복귀)
            FinishAttack(); 
        }
    }
}

void ADelveEnemy_Jumper::ApplyLandingDamage()
{
    FVector Origin = GetActorLocation();
    float Radius = 150.0f;
    
    // 디버그 원 그리기 (빨간색)
    // DrawDebugSphere(GetWorld(), Origin, Radius, 12, FColor::Red, false, 1.0f);

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(this, Origin, Radius, ObjectTypes, nullptr, {}, OverlappedActors);

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor && HitActor != this)
        {
            FDamageEvent DamageEvent;
            HitActor->TakeDamage(Damage, DamageEvent, GetController(), this);
        }
    }
}