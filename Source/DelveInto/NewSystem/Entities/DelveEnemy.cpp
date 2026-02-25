#include "DelveEnemy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PaperSprite.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemies/DelveEnemy_Jumper.h"
#include "NewSystem/DelveAIController.h"
#include "NewSystem/Widgets/HealthBarWidget.h"

// [신규] 플레이어와 인벤토리 정보에 접근하기 위한 헤더
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h"
#include "NewSystem/Widgets/HandDisplayWidget.h"

class ADelveEnemy_Jumper;

ADelveEnemy::ADelveEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // -------------------------------------------------------------
    // 1. 루트 캡슐 (이동 담당)
    // -------------------------------------------------------------
    GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

    EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
    EnemyFlipbook->SetupAttachment(GetCapsuleComponent());
    EnemyFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    
    // -------------------------------------------------------------
    // 2. 몸통 콜리전 (피격 담당)
    // -------------------------------------------------------------
    BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
    BodyCollision->SetupAttachment(GetCapsuleComponent()); 
    BodyCollision->InitBoxExtent(FVector(40.0f, 20.0f, 15.0f));

    BodyCollision->SetCanEverAffectNavigation(false);
    BodyCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BodyCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    BodyCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
    BodyCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(GetCapsuleComponent());
    HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f)); 
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawSize(FVector2D(100.0f, 15.0f));
}

void ADelveEnemy::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    if (EnemyFlipbook && IdleFlipbook) EnemyFlipbook->SetFlipbook(IdleFlipbook);

    if (HealthBarWidget)
    {
        UHealthBarWidget* Bar = Cast<UHealthBarWidget>(HealthBarWidget->GetUserWidgetObject());
        if (Bar)
        {
            Bar->UpdateHealthRatio(1.0f); 
        }
    }
}

void ADelveEnemy::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (!GetCapsuleComponent()) return;

    float CapRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
    float CapHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

    if (BodyCollision)
    {
        float ExtentX = 10.0f / 2.0f;
        float ExtentY = CapRadius;
        float ExtentZ = CapHalfHeight;

        BodyCollision->SetBoxExtent(FVector(ExtentX, ExtentY, ExtentZ));
        BodyCollision->SetRelativeLocation(FVector::ZeroVector);
        BodyCollision->SetRelativeRotation(FRotator::ZeroRotator);
    }

    if (EnemyFlipbook && EnemyFlipbook->GetFlipbook())
    {
        EnemyFlipbook->SetRelativeLocation(FVector(0.0f, 0.0f, -CapHalfHeight));
        EnemyFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

        if (bAutoResizeToCapsule)
        {
            EnemyFlipbook->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
            EnemyFlipbook->UpdateBounds();

            float SpriteWorldHeight = EnemyFlipbook->Bounds.BoxExtent.Z * 2.0f;
            float CapsuleTotalHeight = CapHalfHeight * 2.0f;

            if (SpriteWorldHeight > 1.0f) 
            {
                float NewScale = CapsuleTotalHeight / SpriteWorldHeight;
                EnemyFlipbook->SetRelativeScale3D(FVector(NewScale, NewScale, NewScale));
            }
        }
    }
}

void ADelveEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateAnimation();

    if (bAlwaysFacePlayer && !bIsDead && IsPlayerInDetectRange())
    {
        FaceToPlayer(DeltaTime);
    }
}

void ADelveEnemy::FaceToPlayer(float DeltaTime)
{
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    FVector Direction = Player->GetActorLocation() - GetActorLocation();
    Direction.Z = 0.0f; 

    if (Direction.IsNearlyZero()) return;

    FRotator TargetRotation = Direction.Rotation();
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);
}

void ADelveEnemy::UpdateAnimation()
{
    if (bIsDead || bIsAttacking) return;

    FVector Velocity = GetVelocity();
    float Speed = Velocity.Size();

    UPaperFlipbook* DesiredFlipbook = IdleFlipbook;
    if (Speed > 0.1f && ForwardFlipbook) DesiredFlipbook = ForwardFlipbook;

    if (EnemyFlipbook && EnemyFlipbook->GetFlipbook() != DesiredFlipbook)
    {
        EnemyFlipbook->SetFlipbook(DesiredFlipbook);
        EnemyFlipbook->SetLooping(true);
        EnemyFlipbook->Play();
    }
}

void ADelveEnemy::StartAttackSequence(AActor* Target)
{
    if (bIsDead || bIsAttacking || !bCanAttack || !Target) return;

    bIsAttacking = true;
    bCanAttack = false;
    CachedTarget = Target;

    float PrepDuration = 0.5f; 

    if (EnemyFlipbook && AttackPrepFlipbook)
    {
        EnemyFlipbook->SetFlipbook(AttackPrepFlipbook);
        EnemyFlipbook->SetLooping(false); 
        EnemyFlipbook->PlayFromStart();
        PrepDuration = AttackPrepFlipbook->GetTotalDuration();
    }
    else
    {
        PrepDuration = 0.2f; 
    }

    GetWorld()->GetTimerManager().SetTimer(AttackPrepTimer, this, &ADelveEnemy::ExecuteAttack, PrepDuration, false);
}

void ADelveEnemy::ExecuteAttack()
{
    if (bIsDead) return;

    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }

    float ActionDuration = 0.5f;
    if (EnemyFlipbook && AttackFlipbook)
    {
        EnemyFlipbook->SetFlipbook(AttackFlipbook);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        ActionDuration = AttackFlipbook->GetTotalDuration();
    }

    PerformMeleeDamageCheck();
    GetWorld()->GetTimerManager().SetTimer(AttackActionTimer, this, &ADelveEnemy::FinishAttack, ActionDuration, false);
}

bool ADelveEnemy::IsPlayerInDetectRange() const
{
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return false;

    float Dist = GetDistanceTo(Player);
    return Dist <= DetectRange;
}

void ADelveEnemy::FinishAttack()
{
    if (bIsDead) return;

    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    
    if (Player)
    {
        float MyRadius = (GetCapsuleComponent()) ? GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0f;
        float TargetRadius = 0.0f;

        if (ACharacter* PlayerChar = Cast<ACharacter>(Player))
        {
            if (PlayerChar->GetCapsuleComponent())
            {
                TargetRadius = PlayerChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
            }
        }

        float CenterDist = GetDistanceTo(Player);
        float SurfaceDist = FMath::Max(0.0f, CenterDist - (MyRadius + TargetRadius));
        float ChainAttackRange = AttackRange + 5.0f; 

        if (ADelveEnemy_Jumper* Jumper = Cast<ADelveEnemy_Jumper>(this))
        {
            ChainAttackRange = Jumper->JumpAttackRange;
        }
        
        if (SurfaceDist <= ChainAttackRange +5.0)
        {
            bIsAttacking = false; 
            bCanAttack = true; 
            StartAttackSequence(Player);
            return; 
        }
    }

    ReturnToIdle();
    ResetCooldown();
}

void ADelveEnemy::PerformMeleeDamageCheck()
{
    FVector Start = GetActorLocation() + GetActorForwardVector() * 50.0f;
    float Radius = 60.0f;

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(this, Start, Radius, ObjectTypes, nullptr, {}, OverlappedActors);

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor && HitActor != this)
        {
            if (HitActor->IsA(ADelveEnemy::StaticClass())) 
            {
                continue; 
            }

            FDamageEvent DamageEvent;
            HitActor->TakeDamage(Damage, DamageEvent, GetController(), this);
        }
    }
}

void ADelveEnemy::ReturnToIdle()
{
    if (!bIsDead && IdleFlipbook && EnemyFlipbook)
    {
        EnemyFlipbook->SetFlipbook(IdleFlipbook);
        EnemyFlipbook->SetLooping(true);
        EnemyFlipbook->Play();
    }
}

void ADelveEnemy::ResetCooldown()
{
    bCanAttack = true;
    bIsAttacking = false; 
}

float ADelveEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.0f;

    if (DamageCauser && DamageCauser->IsA(ADelveEnemy::StaticClass()))
    {
        return 0.0f;
    }

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    UE_LOG(LogTemp, Warning, TEXT("[%s] 피격 데미지: %f | 남은 체력: %f / %f"), *GetName(), ActualDamage, CurrentHealth, MaxHealth);

    if (EnemyFlipbook)
    {
        EnemyFlipbook->SetSpriteColor(FLinearColor::Red); 
        GetWorld()->GetTimerManager().ClearTimer(HitFlashTimer);
        GetWorld()->GetTimerManager().SetTimer(HitFlashTimer, this, &ADelveEnemy::ResetSpriteColor, 0.5f, false);
    }

    if (HealthBarWidget)
    {
        UHealthBarWidget* Bar = Cast<UHealthBarWidget>(HealthBarWidget->GetUserWidgetObject());
        if (Bar)
        {
            float Ratio = (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
            Bar->UpdateHealthRatio(Ratio);
        }
    }

    // =============================================================
    // 5. 사망 체크 및 처리 (골드 보상 지급 추가!)
    // =============================================================
    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;

        if (OnEnemyDeath.IsBound())
        {
            OnEnemyDeath.Broadcast(this);
        }

        // [신규] 몬스터 처치 보상 (골드) 지급
        // 안전하게 GetPlayerCharacter를 사용해 현재 조종 중인 플레이어를 찾습니다.
        if (ADelveCharacter* PlayerChar = Cast<ADelveCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
        {
            if (PlayerChar->InventoryHandler)
            {
                PlayerChar->InventoryHandler->GoldAmount += DropGoldAmount;
                UE_LOG(LogTemp, Warning, TEXT("[보상] %s 처치! 골드 획득: %d | 현재 골드: %d"), *GetName(), DropGoldAmount, PlayerChar->InventoryHandler->GoldAmount);

                // [신규] 인벤토리 위젯이 아닌 메인 HUD(WeaponWidgetInstance)의 골드를 직접 갱신!
                if (PlayerChar->WeaponWidgetInstance)
                {
                    PlayerChar->WeaponWidgetInstance->UpdateGoldDisplay(PlayerChar->InventoryHandler->GoldAmount);
                }
            }
        }

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (BodyCollision) BodyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        if (ADelveAIController* AICon = Cast<ADelveAIController>(GetController()))
        {
            AICon->StopMovement();
        }

        if (HealthBarWidget) HealthBarWidget->SetVisibility(false);

        float DeathDuration = 1.0f; 

        if (DeathSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
        }

        if (EnemyFlipbook && DeathFlipbook)
        {
            EnemyFlipbook->SetFlipbook(DeathFlipbook);
            EnemyFlipbook->SetLooping(false); 
            EnemyFlipbook->PlayFromStart();
            
            DeathDuration = DeathFlipbook->GetTotalDuration();
        }

        GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ADelveEnemy::DestroySelf, DeathDuration, false);
    }

    return ActualDamage;
}

void ADelveEnemy::ResetSpriteColor()
{
    if (EnemyFlipbook) EnemyFlipbook->SetSpriteColor(FLinearColor::White);
}

void ADelveEnemy::PrintDistanceToPlayer()
{
    if (bIsDead) return;
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Player)
    {
        float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
        GEngine->AddOnScreenDebugMessage(100, 1.0f, FColor::Yellow, FString::Printf(TEXT("Enemy Dist: %.1f"), Dist));
    }
}

void ADelveEnemy::DestroySelf()
{
    Destroy();
}