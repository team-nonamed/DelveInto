// DelveProjectile.cpp

#include "DelveProjectile.h"
#include "DelveCharacter.h" 
#include "DelveEnemy.h"     
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h" // [필수] 이펙트 스폰 함수용

ADelveProjectile::ADelveProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 충돌체 설정
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(15.0f);
    
    // 1. 프로필 설정
    CollisionComp->SetCollisionProfileName(TEXT("Projectile")); 

    // 2. [추가] 중요! WorldDynamic(트리거 박스 등)과는 부딪히지 않고 통과하도록 설정
    CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    
    // (선택) 아예 무시하고 싶다면 ECR_Overlap 대신 ECR_Ignore 사용
    // CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

    CollisionComp->OnComponentHit.AddDynamic(this, &ADelveProjectile::OnHit);

    // 2. 나이아가라 컴포넌트 (날아가는 모습)
    ProjectileEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffect"));
    ProjectileEffect->SetupAttachment(RootComponent);
    // ProjectileEffect->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // 이펙트 방향이 이상하면 주석 해제 후 조절

    // 3. 포인트 라이트 (발광 효과)
    ProjectileLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ProjectileLight"));
    ProjectileLight->SetupAttachment(RootComponent);
    ProjectileLight->SetIntensity(5000.0f); // 밝기 조절
    ProjectileLight->SetLightColor(FLinearColor(0.0f, 0.5f, 1.0f)); // 기본 파란색 (BP에서 수정 가능)
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

    // =========================================================================
    // [수정] 변수와 컴포넌트 동기화 (여기가 핵심!)
    // 생성자 이후에 블루프린트에서 'MoveSpeed'를 바꿨을 경우를 대비해
    // 게임 시작 시점에 강제로 속도를 다시 설정해 줍니다.
    // =========================================================================
    if (ProjectileMovement)
    {
        // 1. 컴포넌트 속도 설정 갱신
        ProjectileMovement->MaxSpeed = MoveSpeed;
        ProjectileMovement->InitialSpeed = MoveSpeed;
        
        // 2. 실제 속도(Velocity) 벡터에 즉시 적용 (이게 없으면 처음에 멈춰있을 수 있음)
        ProjectileMovement->Velocity = GetActorForwardVector() * MoveSpeed;
    }

    // [기존 코드] MuzzleFlash 이펙트 재생
    if (MuzzleFlashEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this, 
            MuzzleFlashEffect, 
            GetActorLocation(), 
            GetActorRotation(), 
            FVector(1.0f), 
            true, 
            true, 
            ENCPoolMethod::AutoRelease
        );
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
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

    AActor* MyInstigator = GetInstigator();
    bool bShouldDestroy = false; // 충돌 후 삭제 여부

    // --- 피아식별 및 데미지 로직 ---
    if (MyInstigator && MyInstigator->IsA(ADelveCharacter::StaticClass()))
    {
        // 플레이어가 쏨 -> 적에게만 데미지
        if (OtherActor->IsA(ADelveEnemy::StaticClass()))
        {
            UGameplayStatics::ApplyDamage(OtherActor, CurrentDamage, MyInstigator->GetInstigatorController(), this, UDamageType::StaticClass());
            bShouldDestroy = true;
        }
        else if (!OtherActor->IsA(ADelveCharacter::StaticClass())) // 벽 등
        {
            bShouldDestroy = true;
        }
    }
    else if (MyInstigator && MyInstigator->IsA(ADelveEnemy::StaticClass()))
    {
        // 적이 쏨 -> 플레이어에게만 데미지
        if (OtherActor->IsA(ADelveCharacter::StaticClass()))
        {
            UGameplayStatics::ApplyDamage(OtherActor, CurrentDamage, MyInstigator->GetInstigatorController(), this, UDamageType::StaticClass());
            bShouldDestroy = true;
        }
        else if (!OtherActor->IsA(ADelveEnemy::StaticClass())) // 벽 등
        {
            bShouldDestroy = true;
        }
    }
    else
    {
        bShouldDestroy = true; // 주인 불명일 때
    }

    // --- 삭제 전 비주얼 처리 ---
    if (bShouldDestroy)
    {
        // [로직 이식] 충돌 시 HitEffect 재생 (사진 3번 로직)
        if (HitEffect)
        {
            // 충돌 지점(Hit.ImpactPoint)과 법선 벡터(Hit.ImpactNormal)를 고려해 회전 설정
            FRotator HitRotation = Hit.ImpactNormal.Rotation();
            
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                this, 
                HitEffect, 
                Hit.ImpactPoint, 
                HitRotation, 
                FVector(1.0f), 
                true, 
                true, 
                ENCPoolMethod::AutoRelease
            );
        }

        Destroy();
    }
}