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

    if (PlayerPawn && ControlledEnemy && !ControlledEnemy->bIsDead)
    {
        // [핵심 해결책] 
        // 이미 공격 행동을 하고 있다면(차징 중이거나, 휘두르는 중이면)
        // 이동 명령을 내리지 말고 그대로 함수를 종료합니다.
        // -> 이렇게 해야 공격 모션 중에 바닥을 미끄러지며 따라가는 현상이 사라집니다.
        if (ControlledEnemy->bIsAttacking)
        {
            // (선택 사항) 공격 중에도 적을 바라보게 하려면 아래 줄 추가
            // SetFocus(PlayerPawn); 
            return; 
        }

        // -------------------------------------------------------
        // 이 아래는 "공격 중이 아닐 때"만 실행됩니다.
        // -------------------------------------------------------

        // 1. 캡슐 반지름 가져오기
        float MyRadius = (ControlledEnemy->GetCapsuleComponent()) ? ControlledEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0f;
        float TargetRadius = 0.0f;
        if (ACharacter* PlayerChar = Cast<ACharacter>(PlayerPawn))
        {
            if (PlayerChar->GetCapsuleComponent()) TargetRadius = PlayerChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
        }

        // 2. 거리 계산
        float CenterDist = FVector::Dist(ControlledEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());
        float SurfaceDist = FMath::Max(0.0f, CenterDist - (MyRadius + TargetRadius));

        // 3. 사거리 결정
        float EngagementRange = ControlledEnemy->AttackRange; 
        if (ADelveEnemy_Jumper* Jumper = Cast<ADelveEnemy_Jumper>(ControlledEnemy))
        {
            EngagementRange = Jumper->JumpAttackRange;
        }

        // 4. 이동 및 공격 판정
        // 약간의 오차 허용(+5.0f)
        if (SurfaceDist <= EngagementRange + 5.0f)
        {
            StopMovement(); 
            ControlledEnemy->StartAttackSequence(PlayerPawn);
        }
        else
        {
            // 공격 중이 아닐 때만 이 코드가 실행되므로, 
            // 이제 멀어졌을 때 정상적으로 '걷기 애니메이션'이 나옵니다.
            float MoveTargetRange = EngagementRange * 0.5f; 
            MoveToActor(PlayerPawn, MoveTargetRange); 
        }
    }
}