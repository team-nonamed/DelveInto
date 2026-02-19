#include "DelveProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NewSystem/Utils/CombatUtil.h" // [신규] 유틸리티 참조

ADelveProjectile::ADelveProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 충돌체 설정
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    RootComponent = CollisionComp; // [수정] 충돌체를 Root로 설정하는 것이 물리에 안전합니다.
    CollisionComp->InitSphereRadius(15.0f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComp->SetNotifyRigidBodyCollision(true);

    //CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    CollisionComp->OnComponentHit.AddDynamic(this, &ADelveProjectile::OnHit);

    // 2. 나이아가라 컴포넌트
    ProjectileEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffect"));
    ProjectileEffect->SetupAttachment(RootComponent);

    // 3. 포인트 라이트
    ProjectileLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ProjectileLight"));
    ProjectileLight->SetupAttachment(RootComponent);
    ProjectileLight->SetIntensity(5000.0f);
    ProjectileLight->SetLightColor(FLinearColor(0.0f, 0.5f, 1.0f));
    ProjectileLight->AttenuationRadius = 300.0f;

    // 4. 이동 컴포넌트
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = MoveSpeed;
    ProjectileMovement->MaxSpeed = MoveSpeed;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;

    CurrentDamage = BaseDamage;
}

void ADelveProjectile::BeginPlay()
{
    Super::BeginPlay();
    SetLifeSpan(LifeTime);

    if (ProjectileMovement)
    {
        ProjectileMovement->MaxSpeed = MoveSpeed;
        ProjectileMovement->InitialSpeed = MoveSpeed;
        ProjectileMovement->Velocity = GetActorForwardVector() * MoveSpeed;
    }

    if (MuzzleFlashEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MuzzleFlashEffect, GetActorLocation(), GetActorRotation());
    }
}

void ADelveProjectile::InitializeChargeStats(float ChargeRatio)
{
    float Ratio = FMath::Clamp(ChargeRatio, 0.0f, 1.0f);
    CurrentDamage = FMath::Lerp(MinDamage, MaxDamage, Ratio);
    float NewSpeed = FMath::Lerp(MinSpeed, MaxSpeed, Ratio);

    if (ProjectileMovement)
    {
        ProjectileMovement->MaxSpeed = NewSpeed;
        ProjectileMovement->Velocity = GetActorForwardVector() * NewSpeed;
    }
}

void ADelveProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AActor* MyInstigator = GetInstigator();
    
    if (!OtherActor || OtherActor == this || OtherActor == MyInstigator) return;

    // [핵심] 1. 아군이면 무시하고 그냥 통과! (파괴 안 됨)
    if (UCombatUtil::IsFriendly(MyInstigator, OtherActor))
    {
        return; 
    }

    // [핵심] 2. 데미지 적용 시도 (IsEnemy를 내부에서 검사하여 적일 때만 데미지 들어감)
    UCombatUtil::ApplyDamageIfEnemy(this, MyInstigator, OtherActor, CurrentDamage, this);

    // 3. 적이든 벽(중립)이든 부딪혔으므로 이펙트 터뜨리고 소멸
    if (HitEffect)
    {
        FRotator HitRotation = Hit.ImpactNormal.Rotation();
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, Hit.ImpactPoint, HitRotation);
    }

    Destroy();
}