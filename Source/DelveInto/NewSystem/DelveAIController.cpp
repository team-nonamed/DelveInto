#include "DelveAIController.h"
#include "Kismet/GameplayStatics.h"
#include "DelveEnemy.h"
#include "DelveEnemy_Jumper.h"

void ADelveAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ADelveAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 제어 중인 폰을 기본 Enemy로 캐스팅
	ADelveEnemy* BaseEnemy = Cast<ADelveEnemy>(GetPawn());
	if (!BaseEnemy || BaseEnemy->bIsDead) 
	{
		StopMovement();
		return;
	}

	// 플레이어 다시 찾기 (만약 없다면)
	if (!PlayerPawn) PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	// 거리 계산
	float Distance = FVector::Dist(BaseEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());

	// 공격 중이면 이동 금지 (공통)
	if (BaseEnemy->bIsAttacking)
	{
		StopMovement();
		return;
	}

	// --- 행동 결정 ---

	// 1. 기본 공격 (초근접)
	if (Distance <= BaseEnemy->AttackRange)
	{
		StopMovement();
		// 플레이어 보기
		FVector Dir = PlayerPawn->GetActorLocation() - BaseEnemy->GetActorLocation();
		Dir.Z = 0;
		BaseEnemy->SetActorRotation(Dir.Rotation());
        
		BaseEnemy->Attack();
	}
	else
	{
		// 2. 점프 공격 시도 (Jumper인지 확인)
		ADelveEnemy_Jumper* Jumper = Cast<ADelveEnemy_Jumper>(BaseEnemy);
        
		// Jumper가 맞고, 거리가 적당하고, 너무 가깝지 않다면(150 이상)
		if (Jumper && Distance <= Jumper->JumpAttackRange && Distance > 150.0f)
		{
			StopMovement();
			// 플레이어 보기
			FVector Dir = PlayerPawn->GetActorLocation() - BaseEnemy->GetActorLocation();
			Dir.Z = 0;
			BaseEnemy->SetActorRotation(Dir.Rotation());
            
			Jumper->JumpAttack(PlayerPawn);
		}
		// 3. 추적 (너무 멀면)
		else if (Distance < 1500.0f)
		{
			MoveToActor(PlayerPawn, 50.0f);
		}
		else
		{
			StopMovement();
		}
	}
}