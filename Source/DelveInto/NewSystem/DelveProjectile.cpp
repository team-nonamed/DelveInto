#include "DelveProjectile.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ADelveProjectile::ADelveProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. 비주얼 설정
	ProjectileFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ProjectileFlipbook"));
	RootComponent = ProjectileFlipbook;

	// 2. 이동 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 0.0f; // 나중에 세팅
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 검기는 일직선으로 날아감
}

void ADelveProjectile::InitializeChargeStats(float ChargeRatio)
{
	if (!ProjectileMovement) return;

	// 차징 비율(0~1)에 따라 최종 수치 계산
	float FinalSpeed = FMath::Lerp(MinSpeed, MaxSpeed, ChargeRatio);
	float FinalLifeSpan = FMath::Lerp(MinLifeSpan, MaxLifeSpan, ChargeRatio);

	// 속도 적용
	ProjectileMovement->Velocity = GetActorForwardVector() * FinalSpeed;
	ProjectileMovement->InitialSpeed = FinalSpeed;

	// 수명 적용 (지정된 시간 후 자동 파괴)
	SetLifeSpan(FinalLifeSpan);
}