#include "DelvePlayerProjectile.h"
#include "DelveEnemy.h" // 적을 타격해야 하므로 헤더 포함
#include "Kismet/GameplayStatics.h"

ADelvePlayerProjectile::ADelvePlayerProjectile()
{
    PrimaryActorTick.bCanEverTick = false; // 투사체는 Tick이 굳이 필요 없음 (Movement 컴포넌트가 알아서 함)

    // 1. 충돌체 설정
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(20.0f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile")); // 혹은 OverlapAllDynamic 후 로직 처리
    
    // 플레이어 투사체는 적(Pawn)과 WorldDynamic, WorldStatic 등에 막혀야 함
    // 충돌 이벤트 바인딩
    CollisionComp->OnComponentHit.AddDynamic(this, &ADelvePlayerProjectile::OnHit);

    RootComponent = CollisionComp;

    // 2. 비주얼 설정
    ProjectileFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ProjectileFlipbook"));
    ProjectileFlipbook->SetupAttachment(RootComponent);
    ProjectileFlipbook->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 검기(Wave)가 나가는 것이라면 보통 눕혀져 있거나 방향 회전이 필요함
    ProjectileFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    // 3. 이동 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = TravelSpeed;
    ProjectileMovement->MaxSpeed = TravelSpeed;
    ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 스프라이트 회전
    ProjectileMovement->ProjectileGravityScale = 0.0f;   // 중력 없이 직선으로 날아감

    // 기본값 초기화
    CurrentDamage = MinDamage;
}

void ADelvePlayerProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    // 안전장치: 혹시 InitializeProjectile이 호출 안 됐을 경우를 대비해 기본 수명 설정
    // 기본적으로 MinDistance만큼 날아가고 사라짐
    if (TravelSpeed > 0.0f)
    {
        SetLifeSpan(MinDistance / TravelSpeed);
    }
    else
    {
        SetLifeSpan(2.0f);
    }
}

void ADelvePlayerProjectile::InitializeProjectile(float ChargeRatio)
{
    // 0.0 ~ 1.0 범위로 안전하게 자름
    float Ratio = FMath::Clamp(ChargeRatio, 0.0f, 1.0f);

    // 1. 데미지 계산 (Lerp: 선형 보간)
    // Ratio가 0이면 MinDamage, 1이면 MaxDamage, 0.5면 중간값
    CurrentDamage = FMath::Lerp(MinDamage, MaxDamage, Ratio);

    // 2. 사거리(Distance) 계산
    float TargetDistance = FMath::Lerp(MinDistance, MaxDistance, Ratio);

    // 3. 수명(LifeSpan) 설정
    // 거리 = 속도 * 시간  =>  시간 = 거리 / 속도
    if (TravelSpeed > 0.0f)
    {
        float LifeTime = TargetDistance / TravelSpeed;
        SetLifeSpan(LifeTime); 
        
        // 이동 컴포넌트 속도도 혹시 모르니 동기화
        ProjectileMovement->InitialSpeed = TravelSpeed;
        ProjectileMovement->MaxSpeed = TravelSpeed;
    }

    // 디버깅용 로그 (필요시 주석 해제)
    // UE_LOG(LogTemp, Warning, TEXT("Player Projectile Fired! Ratio: %.2f, Dmg: %.1f, Dist: %.1f"), Ratio, CurrentDamage, TargetDistance);
}

void ADelvePlayerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 유효성 검사
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != GetOwner()))
    {
        // 1. 적(Enemy)을 맞췄을 때만 데미지
        if (OtherActor->IsA(ADelveEnemy::StaticClass()))
        {
            // 데미지 전달
            UGameplayStatics::ApplyDamage(OtherActor, CurrentDamage, GetInstigatorController(), this, UDamageType::StaticClass());
            
            // (선택) 타격감 있는 이펙트 스폰
            /*
            if (HitEffectFlipbook)
            {
               // UPaperFlipbookComponent를 일회성으로 생성해서 재생하거나, 별도 이펙트 액터 스폰
            }
            */
            
            // 적을 맞추면 관통할지, 사라질지 결정 (여기선 사라짐)
            Destroy();
        }
        // 2. 벽이나 지형(WorldStatic)에 맞았을 때
        else 
        {
            // 그냥 벽에 부딪혀서 사라짐
            Destroy();
        }
    }
}