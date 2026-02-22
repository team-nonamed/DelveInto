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

class ADelveEnemy_Jumper;

ADelveEnemy::ADelveEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // -------------------------------------------------------------
    // 1. 루트 캡슐 (이동 담당)
    // -------------------------------------------------------------
    GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);

    // 이동용 캡슐은 공격 무시
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

    // 플립북 설정
    EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
    EnemyFlipbook->SetupAttachment(GetCapsuleComponent());
    EnemyFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    
    // -------------------------------------------------------------
    // 2. 몸통 콜리전 (피격 담당, 박스 형태)
    // -------------------------------------------------------------
    BodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));
    
    // [수정된 부분] 2D 캐릭터는 3D Mesh(뼈)가 없으므로 캡슐(Root)에 직접 붙입니다.
    BodyCollision->SetupAttachment(GetCapsuleComponent()); 

    // 박스 크기 초기값
    BodyCollision->InitBoxExtent(FVector(40.0f, 20.0f, 15.0f));

    // =============================================================
    // [핵심] 내비게이션 방해 금지 설정
    // =============================================================
    
    // 1. 길찾기(NavMesh)에 구멍 뚫지 않기 (필수!)
    BodyCollision->SetCanEverAffectNavigation(false);

    // 2. 충돌 검사(Query)만 켜기
    BodyCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // 3. 일단 모두 무시
    BodyCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

    // 4. 공격만 막음 (Camera = 투사체/공격 판정)
    BodyCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
    BodyCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(GetCapsuleComponent());
    
    // 머리 위로 위치 조정 (약간 위로 띄움)
    HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f)); 
    
    // [중요] 'Screen' 모드로 설정하면 카메라를 항상 바라보며 크기가 일정하게 유지됨
    // 'World' 모드로 하면 거리가 멀어지면 작아짐 (취향껏 선택)
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    
    // 위젯 크기 설정
    HealthBarWidget->SetDrawSize(FVector2D(100.0f, 15.0f));
}


void ADelveEnemy::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    if (EnemyFlipbook && IdleFlipbook) EnemyFlipbook->SetFlipbook(IdleFlipbook);

    // [추가] 시작하자마자 체력바 100%로 초기화
    // 위젯 컴포넌트가 초기화될 시간을 주기 위해 약간의 딜레이가 필요할 수도 있지만,
    // 보통 BeginPlay 시점에는 생성되어 있음.
    if (HealthBarWidget)
    {
        // 위젯 컴포넌트에서 실제 위젯 객체 가져오기 (Cast 필요)
        UHealthBarWidget* Bar = Cast<UHealthBarWidget>(HealthBarWidget->GetUserWidgetObject());
        if (Bar)
        {
            Bar->UpdateHealthRatio(1.0f); // 100%
        }
    }
    
    // 디버그용 (필요 없으면 삭제)
    // GetWorld()->GetTimerManager().SetTimer(DistanceDebugTimer, this, &ADelveEnemy::PrintDistanceToPlayer, 1.0f, true);
}


void ADelveEnemy::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // 1. 필수 컴포넌트 확인
    if (!GetCapsuleComponent()) return;

    // 2. 캡슐 크기 정보
    float CapRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
    float CapHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

    // 3. BodyCollision(박스) 크기 동기화 (기존 로직 유지)
    if (BodyCollision)
    {
        float ExtentX = 10.0f / 2.0f;
        float ExtentY = CapRadius;
        float ExtentZ = CapHalfHeight;

        BodyCollision->SetBoxExtent(FVector(ExtentX, ExtentY, ExtentZ));
        BodyCollision->SetRelativeLocation(FVector::ZeroVector);
        BodyCollision->SetRelativeRotation(FRotator::ZeroRotator);
    }

    // 4. [수정됨] 플립북 스케일 자동 조절 (Bounds 방식)
    if (EnemyFlipbook && EnemyFlipbook->GetFlipbook())
    {
        // 위치 및 회전 설정
        EnemyFlipbook->SetRelativeLocation(FVector(0.0f, 0.0f, -CapHalfHeight));
        EnemyFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

        if (bAutoResizeToCapsule)
        {
            // (1) 먼저 스케일을 1.0으로 초기화해야 정확한 '원본 크기'를 잴 수 있음
            EnemyFlipbook->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
            
            // (2) 컴포넌트의 경계(Bounds) 업데이트 강제 실행
            EnemyFlipbook->UpdateBounds();

            // (3) 현재 플립북의 월드 상 높이 측정 (BoxExtent.Z는 절반 높이이므로 * 2)
            // Bounds는 빌드된 게임에서도 정상적으로 작동함
            float SpriteWorldHeight = EnemyFlipbook->Bounds.BoxExtent.Z * 2.0f;
            float CapsuleTotalHeight = CapHalfHeight * 2.0f;

            // (4) 비율 계산 및 적용
            if (SpriteWorldHeight > 1.0f) // 0으로 나누기 방지
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
    // 매 프레임 애니메이션 갱신
    UpdateAnimation();

    // [추가] 항상 플레이어를 바라보게 회전
    if (bAlwaysFacePlayer && !bIsDead && IsPlayerInDetectRange())
    {
        FaceToPlayer(DeltaTime);
    }
}


void ADelveEnemy::FaceToPlayer(float DeltaTime)
{
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    // 1. 플레이어 방향 벡터 계산
    FVector Direction = Player->GetActorLocation() - GetActorLocation();
    Direction.Z = 0.0f; // 높이 차이는 무시 (평면 회전)

    // 방향이 너무 짧으면(거의 겹쳐있으면) 회전 안 함
    if (Direction.IsNearlyZero()) return;

    // 2. 목표 회전값(Rotator) 계산
    FRotator TargetRotation = Direction.Rotation();

    // 3. 현재 회전값에서 목표 회전값으로 부드럽게 보간 (RInterpTo)
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);

    // 4. 적용
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

// ==========================================
// [핵심] 공격 시스템 구현
// ==========================================

void ADelveEnemy::StartAttackSequence(AActor* Target)
{
    UE_LOG(LogTemp, Display, TEXT("Try to Attack"))
    
    if (bIsDead || bIsAttacking || !bCanAttack || !Target) return;

    // 1. 상태 설정
    bIsAttacking = true;
    bCanAttack = false;
    CachedTarget = Target;

    // 2. 차징(준비) 애니메이션 재생
    float PrepDuration = 0.5f; // 기본값

    if (EnemyFlipbook && AttackPrepFlipbook)
    {
        EnemyFlipbook->SetFlipbook(AttackPrepFlipbook);
        EnemyFlipbook->SetLooping(false); 
        EnemyFlipbook->PlayFromStart();
        PrepDuration = AttackPrepFlipbook->GetTotalDuration();
    }
    else
    {
        PrepDuration = 0.2f; // 애니메이션 없으면 짧은 딜레이
    }

    // UE_LOG(LogTemp, Warning, TEXT("Enemy: Start Charging... (%.2f sec)"), PrepDuration);

    // 3. 준비 시간 후 실행(Execute) 예약
    GetWorld()->GetTimerManager().SetTimer(AttackPrepTimer, this, &ADelveEnemy::ExecuteAttack, PrepDuration, false);
}


void ADelveEnemy::ExecuteAttack()
{
    // [기본 동작: 근접 공격]
    // Jumper 클래스는 이 함수를 오버라이드해서 점프함.

    if (bIsDead) return;

    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }

    // 1. 공격 애니메이션
    float ActionDuration = 0.5f;
    if (EnemyFlipbook && AttackFlipbook)
    {
        EnemyFlipbook->SetFlipbook(AttackFlipbook);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        ActionDuration = AttackFlipbook->GetTotalDuration();
    }

    // 2. 근접 데미지 판정
    PerformMeleeDamageCheck();

    // 3. 후딜레이 후 종료 예약
    GetWorld()->GetTimerManager().SetTimer(AttackActionTimer, this, &ADelveEnemy::FinishAttack, ActionDuration, false);
}


bool ADelveEnemy::IsPlayerInDetectRange() const
{
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return false;

    float Dist = GetDistanceTo(Player);
    
    // 만약 이미 공격 중이거나 전투 중이라면 GiveUpRange(더 긴 거리)를 쓰고,
    // 평소 대기 상태라면 DetectRange(짧은 거리)를 씁니다.
    // (여기서는 간단하게 DetectRange만 사용하겠습니다.)
    return Dist <= DetectRange;
}


void ADelveEnemy::FinishAttack()
{
    if (bIsDead) return;

    // 1. 플레이어 확인
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    
    if (Player)
    {
        // -------------------------------------------------------
        // [수정] 캡슐 크기를 고려한 '표면 거리' 계산
        // -------------------------------------------------------
        float MyRadius = (GetCapsuleComponent()) ? GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0f;
        float TargetRadius = 0.0f;

        // 플레이어의 캡슐 반지름 가져오기
        if (ACharacter* PlayerChar = Cast<ACharacter>(Player))
        {
            if (PlayerChar->GetCapsuleComponent())
            {
                TargetRadius = PlayerChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
            }
        }

        float CenterDist = GetDistanceTo(Player);
        
        // 표면 거리 = 중심 거리 - (내 반지름 + 상대 반지름)
        // 0.0f보다 작아지지 않게 Max 처리
        float SurfaceDist = FMath::Max(0.0f, CenterDist - (MyRadius + TargetRadius));


        // -------------------------------------------------------
        // 2. 연속 공격 사거리 결정
        // -------------------------------------------------------
        // 기본은 AttackRange. (AI 컨트롤러와 로직 통일)
        // 약간의 오차 허용(+5.0f 정도)을 주면 더 부드럽게 연계됩니다.
        float ChainAttackRange = AttackRange + 5.0f; 

        // Jumper라면 점프 사거리 사용
        if (ADelveEnemy_Jumper* Jumper = Cast<ADelveEnemy_Jumper>(this))
        {
            ChainAttackRange = Jumper->JumpAttackRange;
        }

        UE_LOG(LogTemp, Warning, TEXT("Chain Check -> Range: %f / SurfaceDist: %f"), ChainAttackRange, SurfaceDist);
        
        // -------------------------------------------------------
        // 3. '표면 거리'가 사거리 안이라면? -> 쉬지 않고 바로 공격!
        // -------------------------------------------------------
        if (SurfaceDist <= ChainAttackRange +5.0)
        {
            // [중요] 바로 공격하려면 상태를 강제로 리셋해줘야 함
            bIsAttacking = false; 
            bCanAttack = true; 

            // 공격 시퀀스 재시작 (차징부터 다시 시작됨)
            StartAttackSequence(Player);
            return; // 여기서 함수 종료 (Idle로 가지 않음)
        }
    }

    // 4. 적이 없거나 멀어졌다면? -> 원래대로 휴식(Idle) 및 쿨타임
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
        // 1. 나 자신은 때리지 않음 (기존 코드)
        if (HitActor && HitActor != this)
        {
            // 2. [추가] 맞은 놈이 적(Enemy)이면 건너뜀!
            if (HitActor->IsA(ADelveEnemy::StaticClass())) 
            {
                continue; 
            }

            // 플레이어라면 데미지!
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
    bIsAttacking = false; // 다시 행동 가능
    // UE_LOG(LogTemp, Warning, TEXT("Enemy: Attack Ready"));
}


// --- 피격 및 기타 ---

float ADelveEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 1. 이미 죽었으면 로직 중단
    if (bIsDead) return 0.0f;

    // 2. 아군 오인 사격 방지
    if (DamageCauser && DamageCauser->IsA(ADelveEnemy::StaticClass()))
    {
        return 0.0f;
    }

    // 3. 데미지 적용
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    // [추가] 피격 받은 데미지와 남은 체력을 로그로 출력
    UE_LOG(LogTemp, Warning, TEXT("[%s] 피격 데미지: %f | 남은 체력: %f / %f"), *GetName(), ActualDamage, CurrentHealth, MaxHealth);

    // =============================================================
    // 피격 효과: 1초간 빨간색으로 변경
    // =============================================================
    if (EnemyFlipbook)
    {
        EnemyFlipbook->SetSpriteColor(FLinearColor::Red); // 빨간색 설정
        
        // 기존 타이머가 돌고 있다면 초기화하고 다시 1초 세팅
        GetWorld()->GetTimerManager().ClearTimer(HitFlashTimer);
        GetWorld()->GetTimerManager().SetTimer(HitFlashTimer, this, &ADelveEnemy::ResetSpriteColor, 0.5f, false);
    }

    // 4. 위젯 업데이트
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
    // 5. 사망 체크 및 처리
    // =============================================================
    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;

        if (OnEnemyDeath.IsBound())
        {
            OnEnemyDeath.Broadcast(this);
        }

        // A. 사망 시에는 피격 빨간색을 즉시 해제하고 원래 색으로 복구 (선택 사항)
        // 죽는 모션이 빨간색인 게 싫으면 아래 주석 해제하세요.
        // ResetSpriteColor(); 
        // GetWorld()->GetTimerManager().ClearTimer(HitFlashTimer);

        // B. 콜리전 끄기
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (BodyCollision) BodyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        // C. AI 멈추기
        if (ADelveAIController* AICon = Cast<ADelveAIController>(GetController()))
        {
            AICon->StopMovement();
        }

        // D. 위젯 숨기기
        if (HealthBarWidget) HealthBarWidget->SetVisibility(false);

        // E. [핵심] 사망 애니메이션 재생 및 삭제 예약
        float DeathDuration = 1.0f; // 애니메이션 없으면 1초 뒤 삭제

        if (DeathSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
        }

        if (EnemyFlipbook && DeathFlipbook)
        {
            EnemyFlipbook->SetFlipbook(DeathFlipbook);
            EnemyFlipbook->SetLooping(false); // 한 번만 재생
            EnemyFlipbook->PlayFromStart();
            
            // 애니메이션 길이 가져오기
            DeathDuration = DeathFlipbook->GetTotalDuration();
        }

        // 애니메이션이 끝날 때쯤 액터 삭제 (Destroy)
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
    // 게임 월드에서 완전히 제거
    Destroy();
}