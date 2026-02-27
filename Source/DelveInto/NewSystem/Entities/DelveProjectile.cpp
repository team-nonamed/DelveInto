#include "DelveProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NewSystem/Utils/CombatUtil.h" 

ADelveProjectile::ADelveProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 루트 구체 (벽 충돌용)
    RootCollider = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollider"));
    RootCollider->InitSphereRadius(5.0f);
    RootCollider->SetCollisionProfileName(TEXT("Projectile")); // 벽은 Block 하도록 설정
    RootCollider->OnComponentHit.AddDynamic(this, &ADelveProjectile::OnHit);
    RootComponent = RootCollider;

    // 2. 자식 캡슐 (적 타격용)
    CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ProjectileCollider"));
    CollisionComp->SetupAttachment(RootCollider);
    
    // [핵심] 캡슐은 벽에 막히면 안 되고, 적과 겹쳐야(Overlap) 합니다!
    // 언리얼 기본 프리셋 중 투사체/캐릭터와 오버랩되는 프리셋을 쓰거나 수동 설정합니다.
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore); // 다 무시하고
    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터(적)만 Overlap!
    
    // Hit가 아니라 Overlap 이벤트를 연결합니다.
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ADelveProjectile::OnOverlap);

    // 3. 이펙트와 라이트도 루트에 부착
    ProjectileEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffect"));
    ProjectileEffect->SetupAttachment(RootCollider);

    ProjectileLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ProjectileLight"));
    ProjectileLight->SetupAttachment(RootCollider);
    ProjectileLight->SetIntensity(5000.0f);
    ProjectileLight->SetLightColor(FLinearColor(0.0f, 0.5f, 1.0f));
    ProjectileLight->AttenuationRadius = 300.0f;

    // 4. 이동 컴포넌트는 전체(RootCollider)를 이동시킵니다.
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = RootCollider; 
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
    // 벽에 부딪히면 데미지 줄 필요 없이 터지기만 하면 됩니다.
    if (HitEffect)
    {
        FRotator HitRotation = Hit.ImpactNormal.Rotation();
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, Hit.ImpactPoint, HitRotation);
    }
    Destroy();
}

// =========================================================
// 몬스터와 닿았을 때 (눕혀놓은 자식 캡슐이 담당!)
// =========================================================
void ADelveProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* MyInstigator = GetInstigator();
    if (!OtherActor || OtherActor == this || OtherActor == MyInstigator) return;

    // 아군 무시
    if (UCombatUtil::IsFriendly(MyInstigator, OtherActor)) return; 

    // 적에게 데미지 적용
    UCombatUtil::ApplyDamageIfEnemy(this, MyInstigator, OtherActor, CurrentDamage, this);

    // 적을 관통하지 않고 터지게 하려면 아래 코드 실행 (관통하려면 Destroy를 빼면 됩니다)
    if (HitEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, GetActorLocation(), GetActorRotation());
    }
    Destroy();
}