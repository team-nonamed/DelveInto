#include "DelveAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h" 
#include "Entities/DelveEnemy.h" // 경로(Entities/ 등)는 프로젝트에 맞게 수정하세요.
#include "Entities/Enemies/DelveEnemy_Jumper.h" 

void ADelveAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
}

void ADelveAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    ADelveEnemy* ControlledEnemy = Cast<ADelveEnemy>(GetPawn());

    // 예외 처리: 적이나 플레이어가 없거나, 적이 죽었으면 아무것도 안 함
    if (!PlayerPawn || !ControlledEnemy || ControlledEnemy->bIsDead)
    {
        StopMovement();
        return;
    }

    // 1. [공격 중 확인]
    if (ControlledEnemy->bIsAttacking)
    {
        return; 
    }

    // 2. [거리 계산] (중심점 간의 거리)
    float CenterDist = FVector::Dist(ControlledEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());

    // 3. [감지 범위 확인]
    if (CenterDist > ControlledEnemy->DetectRange)
    {
        StopMovement();
        return;
    }

    // 4. [표면 거리 계산]
    float MyRadius = (ControlledEnemy->GetCapsuleComponent()) ? ControlledEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0f;
    float TargetRadius = 0.0f;
    
    if (ACharacter* PlayerChar = Cast<ACharacter>(PlayerPawn))
    {
        if (PlayerChar->GetCapsuleComponent()) 
        {
            TargetRadius = PlayerChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
        }
    }

    float SurfaceDist = FMath::Max(0.0f, CenterDist - (MyRadius + TargetRadius));

    // 5. [사거리 결정]
    float EngagementRange = ControlledEnemy->AttackRange; 
    if (ADelveEnemy_Jumper* Jumper = Cast<ADelveEnemy_Jumper>(ControlledEnemy))
    {
        EngagementRange = Jumper->JumpAttackRange;
    }

    // 6. [행동 결정: 공격 vs 이동]
    if (SurfaceDist <= EngagementRange + 5.0f)
    {
        // 사거리 내 진입 -> 강제로 멈추고 공격 시작
        StopMovement(); 
        ControlledEnemy->StartAttackSequence(PlayerPawn);
    }
    else
    {
        // [핵심] 사거리 밖이면 어설픈 거리를 두지 말고 플레이어 코앞(0.0f)을 목표로 추격!
        // 어차피 거리가 좁혀지면 다음 Tick에서 위 if문에 걸려 알아서 멈춥니다.
        MoveToActor(PlayerPawn, 0.0f); 
    }
}