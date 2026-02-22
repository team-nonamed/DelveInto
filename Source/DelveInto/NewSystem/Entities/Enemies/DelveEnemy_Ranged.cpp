#include "DelveEnemy_Ranged.h"

#include "Kismet/GameplayStatics.h"
#include "NewSystem/Entities/DelveProjectile.h"

ADelveEnemy_Ranged::ADelveEnemy_Ranged()
{
	// 원거리 적이므로 인식/공격 사거리를 넉넉하게 설정
	AttackRange = 500.0f;
    
	// 발사 위치 기본값 (캐릭터 앞 40cm)
	MuzzleOffset = FVector(75.0f, 0.0f, 0.0f);
}

void ADelveEnemy_Ranged::ExecuteAttack()
{
	// 죽었으면 공격 불가
	if (bIsDead) return;

	// 1. 공격 애니메이션 재생 (부모 클래스의 변수 활용)
	float ActionDuration = 0.5f;
	if (EnemyFlipbook && AttackFlipbook)
	{
		EnemyFlipbook->SetFlipbook(AttackFlipbook);
		EnemyFlipbook->SetLooping(false);
		EnemyFlipbook->PlayFromStart();
		ActionDuration = AttackFlipbook->GetTotalDuration();
	}

	// 2. 투사체 발사 로직
	if (ProjectileClass && GetWorld())
	{
		// 발사 원점: 내 위치 + 회전된 오프셋
		FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(MuzzleOffset);
		FRotator SpawnRotation = GetActorRotation();

		// 플레이어 방향으로 조준 보정
		AActor* Target = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (Target)
		{
			FVector Direction = Target->GetActorLocation() - SpawnLocation;
			SpawnRotation = Direction.Rotation(); // 타겟을 향해 회전
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;            // 주인은 나(적)
		SpawnParams.Instigator = this;       // 가해자도 나

		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
		
		// 투사체 스폰 (InitializeChargeStats는 호출 안 함 -> 기본 데미지 사용)
		GetWorld()->SpawnActor<ADelveProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	}

	// 3. 후딜레이 후 종료 (부모의 FinishAttack 호출하여 쿨타임/Idle 복귀)
	GetWorld()->GetTimerManager().SetTimer(AttackActionTimer, this, &ADelveEnemy::FinishAttack, ActionDuration, false);
}