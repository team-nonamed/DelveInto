#include "DelveAIController.h"
#include "Kismet/GameplayStatics.h"
#include "DelveEnemy.h"
#include "DelveEnemy_Jumper.h"
#include "Components/CapsuleComponent.h"

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
    // 공격 모션 중이라면 이동 명령을 내리지 않고 함수 종료 (미끄러짐 방지)
    if (ControlledEnemy->bIsAttacking)
    {
        return; 
    }

    // 2. [거리 계산] (중심점 간의 거리)
    float CenterDist = FVector::Dist(ControlledEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());

    // 3. [감지 범위 확인 - 핵심 추가 사항]
    // 플레이어가 감지 거리(DetectRange)보다 멀리 있다면?
    if (CenterDist > ControlledEnemy->DetectRange)
    {
        // 쫓아가던 중이었다면 멈추고 함수 종료 (Idle 상태 유지)
        StopMovement();
        
        // (선택 사항) 만약 적이 멀어졌을 때 Idle 플립북으로 확실히 돌아가게 하려면:
        // ControlledEnemy->ReturnToIdle(); 
        return;
    }

    // -------------------------------------------------------
    // 이 아래는 "공격 중도 아니고" + "감지 범위 안일 때"만 실행됩니다.
    // -------------------------------------------------------

    // 4. [표면 거리 계산] (공격 판정용 정밀 거리)
    float MyRadius = (ControlledEnemy->GetCapsuleComponent()) ? ControlledEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0f;
    float TargetRadius = 0.0f;
    
    if (ACharacter* PlayerChar = Cast<ACharacter>(PlayerPawn))
    {
        if (PlayerChar->GetCapsuleComponent()) 
        {
            TargetRadius = PlayerChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
        }
    }

    // 표면 거리 = 중심 거리 - (내 반지름 + 상대 반지름)
    float SurfaceDist = FMath::Max(0.0f, CenterDist - (MyRadius + TargetRadius));

    // 5. [사거리 결정] (점퍼 여부 확인)
    float EngagementRange = ControlledEnemy->AttackRange; 
    if (ADelveEnemy_Jumper* Jumper = Cast<ADelveEnemy_Jumper>(ControlledEnemy))
    {
        EngagementRange = Jumper->JumpAttackRange;
    }

    // 6. [행동 결정: 공격 vs 이동]
    // 약간의 오차(+5.0f)를 두어 진동 방지
    if (SurfaceDist <= EngagementRange + 5.0f)
    {
        // 사거리 내 진입 -> 멈추고 공격 시작
        StopMovement(); 
        ControlledEnemy->StartAttackSequence(PlayerPawn);
    }
    else
    {
        // 사거리 밖 -> 추격
        // MoveToActor를 사용할 때, 목표 지점을 사거리보다 살짝 안쪽(0.9배 정도)으로 잡으면 더 자연스럽게 멈춥니다.
        // EngagementRange * 0.5f는 너무 가깝게 붙으려 할 수 있으니, 상황에 따라 조절하세요.
        // 여기서는 기존 로직(0.5f)을 유지하되, -10.0f 정도로 여유를 두는 것도 추천합니다.
        MoveToActor(PlayerPawn, EngagementRange - 10.0f); 
    }
}