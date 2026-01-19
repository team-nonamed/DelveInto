#include "DelveEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PaperSprite.h" // <--- [필수] 이거 꼭 추가해주세요!

ADelveEnemy::ADelveEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // [1. 생성자에서 컴포넌트 만들기]
    // "EnemyFlipbook"이라는 이름으로 컴포넌트 생성
    EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
    
    // 캡슐 컴포넌트(Root)에 자식으로 갖다 붙임
    EnemyFlipbook->SetupAttachment(GetCapsuleComponent());

    // (선택 사항) 스프라이트 위치나 회전 기본값 잡기
    // 보통 2D 게임은 캐릭터가 정면을 보게 90도 돌리거나 함
    EnemyFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

void ADelveEnemy::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    
    if (EnemyFlipbook && IdleFlipbook)
    {
        EnemyFlipbook->SetFlipbook(IdleFlipbook);
    }

    // 2. 거리 측정 타이머 (1초마다 실행)
    GetWorld()->GetTimerManager().SetTimer(DistanceDebugTimer, this, &ADelveEnemy::PrintDistanceToPlayer, 1.0f, true);
}

void ADelveEnemy::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (!EnemyFlipbook || !GetCapsuleComponent()) return;

    // 1. 캡슐 정보 가져오기
    float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    float CapsuleTotalHeight = HalfHeight * 2.0f;

    // 2. 위치 자동 조절 (발바닥 맞추기)
    EnemyFlipbook->SetRelativeLocation(FVector(0.0f, 0.0f, -HalfHeight));
    EnemyFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    // 3. 스케일 자동 조절
    if (bAutoResizeToCapsule && EnemyFlipbook->GetFlipbook())
    {
        UPaperFlipbook* CurrentFlipbook = EnemyFlipbook->GetFlipbook();
        
        // [수정됨] 플립북 자체가 아니라, 플립북 안의 '첫 번째 스프라이트'를 가져옵니다.
        if (CurrentFlipbook->GetNumKeyFrames() > 0)
        {
            UPaperSprite* FirstSprite = CurrentFlipbook->GetSpriteAtFrame(0);

            if (FirstSprite)
            {
                // 스프라이트에서 정보 추출
                float SourcePixelHeight = FirstSprite->GetSourceSize().Y;
                float PPU = FirstSprite->GetPixelsPerUnrealUnit();
                
                if (PPU <= 0.0f) PPU = 1.0f; 

                float SpriteWorldHeight = SourcePixelHeight / PPU;

                // 비율 계산
                if (SpriteWorldHeight > 0.0f)
                {
                    float NewScale = CapsuleTotalHeight / SpriteWorldHeight;
                    EnemyFlipbook->SetRelativeScale3D(FVector(NewScale, NewScale, NewScale));
                }
            }
        }
    }
}

void ADelveEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 매 프레임 애니메이션 갱신
    UpdateAnimation();
}

void ADelveEnemy::UpdateAnimation()
{
    // 1. 우선순위 체크: 죽었거나 공격 중이면 걷기/대기 모션으로 바꾸지 않음
    if (bIsDead || bIsAttacking) return;

    // 2. 현재 속도(Velocity) 가져오기
    FVector Velocity = GetVelocity();
    float Speed = Velocity.Size(); // 속도의 크기 (0이면 정지, 0보다 크면 이동)

    // 3. 결정할 플립북 선택
    UPaperFlipbook* DesiredFlipbook = IdleFlipbook; // 기본은 Idle

    if (Speed > 0.1f) // 아주 미세한 움직임은 무시하려고 0.1보다 클 때로 설정
    {
        if (ForwardFlipbook) DesiredFlipbook = ForwardFlipbook;
    }

    // 4. [중요] 플립북 교체 (이미 재생 중인 것과 다를 때만 SetFlipbook 호출)
    // 매 프레임 SetFlipbook을 하면 애니메이션이 1프레임에서 계속 리셋되어 렉 걸린 것처럼 보임
    if (EnemyFlipbook && EnemyFlipbook->GetFlipbook() != DesiredFlipbook)
    {
        EnemyFlipbook->SetFlipbook(DesiredFlipbook);
        EnemyFlipbook->SetLooping(true); // 걷기와 대기는 반복 재생
        EnemyFlipbook->Play();
    }
}

// --- 공격 시스템 ---

void ADelveEnemy::Attack()
{
    if (bIsDead || bIsAttacking || !bCanAttack) return;

    bIsAttacking = true;
    bCanAttack = false;

    // 1. 애니메이션 재생
    float AnimDuration = 0.5f;
    if (AttackFlipbook && EnemyFlipbook)
    {
        EnemyFlipbook->SetFlipbook(AttackFlipbook);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        AnimDuration = AttackFlipbook->GetTotalDuration();
    }

    UE_LOG(LogTemp, Warning, TEXT("Enemy Attacks!"));

    // 2. 데미지 판정 (약간의 딜레이 후 실행하려면 Timer 사용, 여기선 즉시 실행)
    PerformAttackCheck();

    // 3. 타이머 설정
    // 애니메이션 복귀
    GetWorld()->GetTimerManager().SetTimer(AnimResetTimer, this, &ADelveEnemy::ReturnToIdle, AnimDuration, false);
    // 쿨타임 리셋
    GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ADelveEnemy::ResetCooldown, AttackCooldown, false);
}

void ADelveEnemy::PerformAttackCheck()
{
    // 심플한 구체 판정 (전방 100 거리)
    FVector Start = GetActorLocation() + GetActorForwardVector() * 50.0f;
    float Radius = 60.0f;
    float Damage = 10.0f;

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(this, Start, Radius, ObjectTypes, nullptr, {}, OverlappedActors);

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor && HitActor != this)
        {
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
    bIsAttacking = false; // 이동 가능 상태로 복귀
}

void ADelveEnemy::ResetCooldown()
{
    bCanAttack = true;
}

// --- 피격 및 사망 시스템 ---

float ADelveEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.0f;

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    // 빨간색 점멸 효과 (죽지 않았을 때만 하거나, 죽어도 잠깐 번쩍이게 유지)
    if (EnemyFlipbook)
    {
        EnemyFlipbook->SetSpriteColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
        GetWorld()->GetTimerManager().ClearTimer(HitFlashTimer);
        GetWorld()->GetTimerManager().SetTimer(HitFlashTimer, this, &ADelveEnemy::ResetSpriteColor, 0.2f, false);
    }

    // 사망 처리
    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true; // [중요] 이 플래그가 true면 UpdateAnimation 등에서 다른 모션으로 안 바뀌게 막아야 함
        
        // 2. AI 이동 중지
        if (GetController()) GetController()->StopMovement();
        
        // 3. 색상 즉시 복구 (선택 사항: 피격 빨간색을 끄고 싶다면)
        ResetSpriteColor(); 
        
        // 4. [핵심] 사망 애니메이션 재생
        float DeathDelay = 1.0f; // 애니메이션 없을 때를 대비한 기본값

        if (EnemyFlipbook && DeathFlipbook)
        {
            EnemyFlipbook->SetFlipbook(DeathFlipbook);
            EnemyFlipbook->SetLooping(false); // 죽는 모션은 한 번만 재생하고 멈춰야 함 (반복 X)
            EnemyFlipbook->PlayFromStart();

            // 애니메이션 총 길이 가져오기
            DeathDelay = DeathFlipbook->GetTotalDuration();
        }

        UE_LOG(LogTemp, Warning, TEXT("Enemy Died! Destroying in %f seconds."), DeathDelay);

        // 5. 애니메이션 길이 + 약간의 여유(0.2초)를 두고 삭제
        SetLifeSpan(DeathDelay + 0.2f); 
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
        // 화면 좌측 상단에 표시 (Key: 100번)
        GEngine->AddOnScreenDebugMessage(100, 1.0f, FColor::Yellow, FString::Printf(TEXT("Enemy Dist: %.1f"), Dist));
    }
}