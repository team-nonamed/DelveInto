#include "DelveEnemy_Jumper.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void ADelveEnemy_Jumper::JumpAttack(AActor* Target)
{
    if (bIsDead || bIsAttacking || !bCanJumpAttack || !Target) return;

    bIsAttacking = true;
    bCanJumpAttack = false;
    bHasHitTargetInAir = false;
    
    // 타겟 저장 (ExecuteJump에서 쓰기 위해)
    CachedJumpTarget = Target;

    // [1단계] 점프 준비 모션 재생
    float PrepDuration = 1.0f; // 기본값 (애니메이션 없을 때)

    if (EnemyFlipbook && JumpStartFlipbook)
    {
        EnemyFlipbook->SetFlipbook(JumpStartFlipbook);
        EnemyFlipbook->SetLooping(false); // 한 번만 재생
        EnemyFlipbook->PlayFromStart();
        
        // 애니메이션 길이만큼 대기
        PrepDuration = JumpStartFlipbook->GetTotalDuration();
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Jumper: Preparing to Jump... for %f seconds"), PrepDuration);

    // [2단계] 애니메이션이 끝나는 시간에 맞춰서 'ExecuteJump' 실행 예약
    GetWorld()->GetTimerManager().SetTimer(JumpDelayTimer, this, &ADelveEnemy_Jumper::ExecuteJump, PrepDuration, false);
}

void ADelveEnemy_Jumper::ExecuteJump()
{
    // 점프하기 전에 죽었거나 타겟이 사라졌으면 취소
    if (bIsDead || !CachedJumpTarget) return;

    // --- 여기서부터 기존 점프 로직 ---
    
    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = CachedJumpTarget->GetActorLocation();
    FVector Direction = (TargetLoc - MyLoc);
    Direction.Z = 0.0f;
    Direction.Normalize();

    // 발사!
    FVector LaunchVel = (Direction * 600.0f) + FVector(0.0f, 0.0f, 500.0f);
    LaunchCharacter(LaunchVel, true, true);

    // [3단계] 공중 모션으로 변경
    if (EnemyFlipbook && JumpFlipbook)
    {
        EnemyFlipbook->SetFlipbook(JumpFlipbook);
        EnemyFlipbook->SetLooping(true);
        EnemyFlipbook->Play();
    }

    UE_LOG(LogTemp, Warning, TEXT("Jumper: Launch!"));

    // 쿨타임 리셋 예약 (점프 시작 후 3초 뒤)
    GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimer, this, &ADelveEnemy_Jumper::ResetJumpCooldown, AttackCooldown, false);
}

// 2. [추가] 공중 충돌 감지 (몸통 박치기)
void ADelveEnemy_Jumper::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    // 공격 중이 아니거나, 이미 때렸거나, 죽었으면 무시
    if (!bIsAttacking || bHasHitTargetInAir || bIsDead) return;

    // 부딪힌 대상이 플레이어인지 확인
    if (Other && Other == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Jumper: Hit Player in Air!"));
        
        bHasHitTargetInAir = true; // 중복 타격 방지

        // A. 데미지 주기 (몸통 박치기 데미지)
        FDamageEvent DamageEvent;
        Other->TakeDamage(10.0f, DamageEvent, GetController(), this);

        // B. [핵심] 타격 플립북으로 교체
        if (EnemyFlipbook && AttackFlipbook)
        {
            EnemyFlipbook->SetFlipbook(AttackFlipbook);
            EnemyFlipbook->SetLooping(false); // 타격감 있게 한 번만 재생
            EnemyFlipbook->PlayFromStart();
        }
    }
}

// 3. 착지 처리
void ADelveEnemy_Jumper::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit); // 부모 클래스 로직 실행

    // 점프 공격을 하다가 착지한 경우에만 로직 수행
    if (bIsAttacking)
    {
        UE_LOG(LogTemp, Warning, TEXT("Jumper: Landed. Checking next move..."));

        // 착지했으니 쿨타임 강제 종료 (바로 다시 뛸 수 있게)
        GetWorld()->GetTimerManager().ClearTimer(AttackCooldownTimer); // 변수명 확인 필요
        bCanJumpAttack = true;
        
        // 1. 착지 데미지 처리
        ApplyLandingDamage(); 

        // 2. 일단 공격 상태를 끕니다. (그래야 다음 공격 명령이 먹힘)
        bIsAttacking = false;
        
        // 3. [핵심] 바로 다시 점프할지 판단
        // 플레이어 가져오기
        AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

        bool bShouldChainJump = false;

        if (Player && !bIsDead)
        {
            float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

            // 조건: 
            // 1. 쿨타임이 끝났는가? (bCanJumpAttack)
            // 2. 점프 사거리(JumpAttackRange) 안인가?
            // 3. 너무 가깝지 않은가? (최소 거리 150) -> 적용 X
            if (bCanJumpAttack && Dist <= JumpAttackRange /* && Dist > 150.0f */)
            {
                bShouldChainJump = true;
            }
        }

        // 4. 분기 처리
        if (bShouldChainJump)
        {
            // 대기(Idle) 없이 바로 점프 준비 시작!
            UE_LOG(LogTemp, Warning, TEXT(">> Chain Jump!"));
            
            // 방향 다시 맞추기 (착지 후 플레이어가 뒤에 있을 수도 있으니)
            if (Player)
            {
                FVector Dir = Player->GetActorLocation() - GetActorLocation();
                Dir.Z = 0.0f;
                SetActorRotation(Dir.Rotation());
                
                // 점프 함수 호출 (Prep -> Jump 로직이 다시 시작됨)
                JumpAttack(Player);
            }
        }
        else
        {
            // 조건이 안 맞으면 얌전히 Idle로 복귀
            ReturnToIdle();
        }
    }
}

void ADelveEnemy_Jumper::ApplyLandingDamage()
{
    FVector Origin = GetActorLocation();
    float Radius = 150.0f;
    float Damage = 20.0f;

    DrawDebugSphere(GetWorld(), Origin, Radius, 12, FColor::Red, false, 1.0f);

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

void ADelveEnemy_Jumper::ResetJumpCooldown()
{
    bCanJumpAttack = true;
}