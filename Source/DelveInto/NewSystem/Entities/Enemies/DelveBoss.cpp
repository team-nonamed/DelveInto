#include "DelveBoss.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraActor.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"

ADelveBoss::ADelveBoss()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentPattern = EBossAttackPattern::None;

    MaxHealth = 1000.0f;
    DropGoldAmount = 500;
}

void ADelveBoss::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;

    // 1. 일반 몬스터용 머리 위 쬐끄만 체력바는 숨깁니다!
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }

    // 2. 다크소울 스타일 거대 체력바 생성 및 화면에 추가
    if (BossHealthWidgetClass)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            BossHealthWidgetInstance = CreateWidget<UBossHealthWidget>(PC, BossHealthWidgetClass);
            if (BossHealthWidgetInstance)
            {
                BossHealthWidgetInstance->AddToViewport(); // 화면에 띄우기
                BossHealthWidgetInstance->SetBossName(BossName); // 이름 셋팅
                BossHealthWidgetInstance->UpdateHealth(CurrentHealth, MaxHealth); // 꽉 찬 체력 셋팅
            }
        }
    }

    PerformSpawnRoar();
}

void ADelveBoss::PerformSpawnRoar()
{
    // 1. AI가 멋대로 걸어가거나 공격하지 못하도록 공격 상태(bIsAttacking)를 true로 묶어둡니다.
    bIsAttacking = true;
    bCanAttack = false;
    CurrentPattern = EBossAttackPattern::FirstRoar;

    float PrepDuration = 0.5f;

    // 2. 포효 준비 모션 재생
    if (EnemyFlipbook && RoarPrepFlipbook)
    {
        EnemyFlipbook->SetFlipbook(RoarPrepFlipbook);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        PrepDuration = RoarPrepFlipbook->GetTotalDuration();
    }

    // 3. 준비 모션이 끝나면 진짜 포효 실행
    GetWorld()->GetTimerManager().SetTimer(SpawnRoarTimer, this, &ADelveBoss::ExecuteSpawnRoarAction, PrepDuration, false);
}

void ADelveBoss::ExecuteSpawnRoarAction()
{
    if (bIsDead) return;

    float ActionDuration = 0.5f;

    // 1. 포효 액션 모션 재생
    if (EnemyFlipbook && RoarActionFlipbook)
    {
        EnemyFlipbook->SetFlipbook(RoarActionFlipbook);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        ActionDuration = RoarActionFlipbook->GetTotalDuration();
    }

    // 2. 최초 조우 전용 웅장한 사운드 재생
    if (FirstRoarSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FirstRoarSound, GetActorLocation());
        // 애니메이션과 사운드 중 더 긴 시간 동안 대기
        ActionDuration = FMath::Max(ActionDuration, FirstRoarSound->GetDuration());

        // [여기에 추가!] 사운드가 터질 때 화면 효과도 같이 터뜨립니다!
        PlayFirstRoarScreenEffect();
    }

    // 3. 사운드가 끝나면 AI 목줄을 풀어주는 함수 호출
    GetWorld()->GetTimerManager().SetTimer(SpawnRoarTimer, this, &ADelveBoss::FinishSpawnRoar, ActionDuration, false);
}

void ADelveBoss::FinishSpawnRoar()
{
    if (bIsDead) return;

    // [핵심] 포효가 끝났으니 AI가 다시 플레이어를 추격하고 공격할 수 있게 풀어줍니다!
    CurrentPattern = EBossAttackPattern::None;
    bIsAttacking = false; 
    bCanAttack = true;

    ReturnToIdle(); // 기본 상태로 복귀 (이후 틱에서 AI가 알아서 플레이어에게 걸어갑니다)
}

void ADelveBoss::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 1. 돌진 패턴 이동 처리 (기존 코드)
    if (bIsChargingForward && !bIsDead)
    {
        AddMovementInput(ChargeDirection, 1.0f);
        PerformMeleeDamageCheck(); 
    }

    // =========================================================
    // 2. [신규] 걷기 애니메이션 발소리 처리 로직
    // =========================================================
    // 죽지 않았고, 공격 중도 아닐 때 (즉, 걷고 있거나 서 있을 때)
    if (!bIsDead && !bIsAttacking)
    {
        // 보스의 현재 이동 속도를 가져옵니다.
        float Speed = GetVelocity().Size2D();

        // 속도가 있고(걷는 중) 플립북이 정상적으로 있다면
        if (Speed > 10.0f && EnemyFlipbook && EnemyFlipbook->GetFlipbook())
        {
            // 현재 재생 중인 플립북의 '프레임 번호(0, 1, 2...)'를 가져옵니다.
            int32 CurrentFrame = EnemyFlipbook->GetPlaybackPositionInFrames();

            // 프레임이 넘어갔을 때만(1프레임당 1번만) 체크합니다.
            if (CurrentFrame != LastFootstepFrame)
            {
                LastFootstepFrame = CurrentFrame;

                // 블루프린트에서 설정한 '발소리가 나야 하는 프레임'에 도달했다면?
                if (FootstepFrames.Contains(CurrentFrame))
                {
                    if (FootstepSound)
                    {
                        UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());
                    }
                }
            }
        }
        else
        {
            // 멈춰 서 있다면 프레임 기억을 초기화합니다.
            LastFootstepFrame = -1;
        }
    }
}

// =========================================================
// 1. 공격 준비 (StartAttackSequence)
// =========================================================
void ADelveBoss::StartAttackSequence(AActor* Target)
{
    if (bIsDead || bIsAttacking || !bCanAttack || !Target) return;

    bIsAttacking = true;
    bCanAttack = false;
    CachedTarget = Target;

    ChooseNextPattern(); 

    float PrepDuration = 0.5f;
    UPaperFlipbook* SelectedPrepAnim = nullptr;
    USoundBase* SelectedPrepSound = nullptr; // [신규] 재생할 사운드를 담을 변수

    // 패턴에 맞춰 애니메이션과 사운드를 동시에 고릅니다.
    switch (CurrentPattern)
    {
    case EBossAttackPattern::Swipe: SelectedPrepAnim = SwipePrepFlipbook; break;
    case EBossAttackPattern::Slam:  SelectedPrepAnim = SlamPrepFlipbook; break;
    case EBossAttackPattern::Charge:SelectedPrepAnim = ChargePrepFlipbook; break;
        
        // [수정] 두 포효 모두 동일한 포효 준비 애니메이션을 사용
    case EBossAttackPattern::Roar:
    case EBossAttackPattern::FirstRoar:  
        SelectedPrepAnim = RoarPrepFlipbook;  
        // 준비 사운드가 있다면 여기서 SelectedPrepSound = RoarPrepSound; 하시면 됩니다.
        break;
    default: break;
    }

    if (EnemyFlipbook && SelectedPrepAnim)
    {
        EnemyFlipbook->SetFlipbook(SelectedPrepAnim);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        PrepDuration = SelectedPrepAnim->GetTotalDuration();
    }

    // [수정] 패턴 전용 사운드가 설정되어 있으면 틀고, 없으면 부모의 기본 사운드를 틀어줍니다.
    if (SelectedPrepSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SelectedPrepSound, GetActorLocation());
    }
    else if (AttackPrepSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackPrepSound, GetActorLocation());
    }

    GetWorld()->GetTimerManager().SetTimer(AttackPrepTimer, this, &ADelveBoss::ExecuteAttack, PrepDuration, false);
}

void ADelveBoss::ChooseNextPattern()
{
    // 예약된 패턴이 있다면? (돌진 전 포효가 끝난 상황)
    if (ForcedNextPattern != EBossAttackPattern::None)
    {
        CurrentPattern = ForcedNextPattern;
        ForcedNextPattern = EBossAttackPattern::None; // 예약 초기화
        return; 
    }

    // 1(Swipe), 2(Slam), 3(Charge) 중 랜덤 뽑기
    int32 RandomPattern = FMath::RandRange(1, 3);
    
    if (RandomPattern == 3) // 3번 'Charge(대시)'가 당첨되었다면?
    {
        // 대시하기 전에 일반 '포효(Roar)'를 먼저 하도록 설정
        CurrentPattern = EBossAttackPattern::Roar;
        
        // 포효가 끝나면 다음번엔 무조건 '돌진(Charge)' 하도록 예약!
        ForcedNextPattern = EBossAttackPattern::Charge;
    }
    else
    {
        CurrentPattern = static_cast<EBossAttackPattern>(RandomPattern);
    }
}

// =========================================================
// 2. 공격 실행 (ExecuteAttack)
// =========================================================
void ADelveBoss::ExecuteAttack()
{
    if (bIsDead) return;

    float ActionDuration = 0.5f;
    UPaperFlipbook* SelectedActionAnim = nullptr;
    USoundBase* SelectedActionSound = nullptr; // [신규] 재생할 실행 사운드

    // 명단 초기화 (다단히트 버그 방지)
    HitActorsThisAttack.Empty();

    switch (CurrentPattern)
    {
    case EBossAttackPattern::Swipe: 
        SelectedActionAnim = SwipeActionFlipbook; 
        SelectedActionSound = SwipeActionSound;
        PerformSwipe(); 
        break;
    case EBossAttackPattern::Slam:  
        SelectedActionAnim = SlamActionFlipbook; 
        SelectedActionSound = SlamActionSound;
        PerformSlam(); 
        break;
    case EBossAttackPattern::Charge:
        SelectedActionAnim = ChargeActionFlipbook; 
        SelectedActionSound = ChargeActionSound;
        PerformCharge(); 
        break;

        // [신규] 돌진 전 포효 (기존 사운드 사용)
    case EBossAttackPattern::Roar:  
        SelectedActionAnim = RoarActionFlipbook; 
        SelectedActionSound = RoarActionSound; // 대시 전 포효음
        PerformRoar();

        // [여기에 추가!] 대시하기 전 일반 포효를 할 때도 화면 효과 발동!
        PlayRoarScreenEffect();
        break;
    default: break;
    }

    if (SelectedActionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SelectedActionSound, GetActorLocation());
    }
    else if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }

    if (EnemyFlipbook && SelectedActionAnim)
    {
        EnemyFlipbook->SetFlipbook(SelectedActionAnim);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        ActionDuration = SelectedActionAnim->GetTotalDuration();
    }

    // =========================================================
    // [핵심 1] 돌진(Charge) 패턴일 경우, 애니메이션 길이 무시하고 내가 설정한 시간만큼 달림!
    // =========================================================
    if (CurrentPattern == EBossAttackPattern::Charge)
    {
        ActionDuration = CustomChargeDuration;

        // 돌진 시간이 애니메이션보다 길 테니, 돌진하는 동안 애니메이션이 반복(Loop)되게 해줍니다.
        if (EnemyFlipbook) EnemyFlipbook->SetLooping(true); 
    }
    // (기존에 작성하신 Roar 사운드 길이 비교 로직은 이 아래에 그대로 두시면 됩니다)
    else if ((CurrentPattern == EBossAttackPattern::Roar || CurrentPattern == EBossAttackPattern::FirstRoar) && SelectedActionSound)
    {
        float SoundDuration = SelectedActionSound->GetDuration();
        ActionDuration = FMath::Max(ActionDuration, SoundDuration);
    }

    // Slam이 아닐 때만 타이머 실행
    if (CurrentPattern != EBossAttackPattern::Slam)
    {
        GetWorld()->GetTimerManager().SetTimer(AttackActionTimer, this, &ADelveBoss::FinishAttack, ActionDuration, false);
    }
}

// =========================================================
// 패턴별 세부 액션 구현
// =========================================================
void ADelveBoss::PerformSwipe()
{
    PerformMeleeDamageCheck();
}

void ADelveBoss::PerformSlam()
{
    LaunchCharacter(FVector(0.0f, 0.0f, 800.0f), false, true);
    GetWorld()->GetTimerManager().SetTimer(SlamLandTimer, this, &ADelveBoss::LandSlam, 0.5f, false);
}

void ADelveBoss::LandSlam()
{
    if (bIsDead) return;

    // 1. 바닥으로 강하게 내리꽂기
    LaunchCharacter(FVector(0.0f, 0.0f, -1500.0f), false, true);

    // =========================================================
    // [신규] 바닥에 닿는 순간 착지 사운드(폭발음) 쾅! 재생
    // =========================================================
    if (SlamLandSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SlamLandSound, GetActorLocation());
    }

    // 2. 착지(쾅!) 애니메이션 재생
    float LandDuration = 0.5f;
    if (EnemyFlipbook && SlamLandFlipbook)
    {
        EnemyFlipbook->SetFlipbook(SlamLandFlipbook);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        
        LandDuration = SlamLandFlipbook->GetTotalDuration();
    }

    // 3. 데미지 판정 로직 (기존과 동일)
    FVector Start = GetActorLocation();
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(this, Start, SlamHitRadius, ObjectTypes, nullptr, {}, OverlappedActors);
    
    HitActorsThisAttack.Empty();

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor && HitActor != this && !HitActor->IsA(ADelveEnemy::StaticClass()))
        {
            if (!HitActorsThisAttack.Contains(HitActor))
            {
                FDamageEvent DamageEvent;
                HitActor->TakeDamage(Damage * 1.5f, DamageEvent, GetController(), this);
                HitActorsThisAttack.Add(HitActor);
            }
        }
    }

    // 4. 다음 패턴으로 넘어가기
    GetWorld()->GetTimerManager().SetTimer(AttackActionTimer, this, &ADelveBoss::FinishAttack, LandDuration, false);
}

void ADelveBoss::PerformCharge()
{
    bIsChargingForward = true;
    
    if (CachedTarget)
    {
        FVector Dir = CachedTarget->GetActorLocation() - GetActorLocation();
        Dir.Z = 0.0f; 
        ChargeDirection = Dir.GetSafeNormal();
    }
    else
    {
        ChargeDirection = GetActorForwardVector();
    }

    // [수정] 고정값 3.0 대신 내가 설정한 배율 사용
    GetCharacterMovement()->MaxWalkSpeed *= ChargeSpeedMultiplier; 
}

void ADelveBoss::PerformRoar()
{
    //PerformMeleeDamageCheck();
}

// =========================================================
// 3. 공격 종료 (FinishAttack)
// =========================================================
void ADelveBoss::FinishAttack()
{
    if (bIsChargingForward)
    {
        bIsChargingForward = false;
        
        // [수정] 돌진이 끝날 때 원래 속도로 복구
        GetCharacterMovement()->MaxWalkSpeed /= ChargeSpeedMultiplier; 
    }

    HitActorsThisAttack.Empty();
    CurrentPattern = EBossAttackPattern::None;

    // =========================================================
    // [핵심] 예약된 패턴(돌진)이 있다면, 거리/쿨타임 무시하고 즉시 연계!
    // =========================================================
    if (ForcedNextPattern != EBossAttackPattern::None && CachedTarget)
    {
        // 부모 클래스의 쿨타임 대기 상태로 넘어가지 않도록 상태를 수동으로 풀어줍니다.
        bIsAttacking = false;
        bCanAttack = true; 

        // 거리에 상관없이 그 자리에서 즉시 예약된 패턴(돌진)의 준비 모션 시작!
        StartAttackSequence(CachedTarget);
        
        return; // Super::FinishAttack()을 부르지 않고 바로 함수 종료
    }

    Super::FinishAttack(); 
}

// =========================================================
// 4. 사망 처리 및 데스 시퀀스 연출
// =========================================================
float ADelveBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.0f;

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // [신규] 보스 전용 체력바 UI 업데이트!
    if (BossHealthWidgetInstance)
    {
        BossHealthWidgetInstance->UpdateHealth(CurrentHealth, MaxHealth);
    }

    if (bIsDead)
    {
        GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
        StartDeathSequence();
    }

    return ActualDamage;
}

void ADelveBoss::StartDeathSequence()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    // [신규] 보스가 죽었으니 화면에서 체력바를 삭제합니다.
    if (BossHealthWidgetInstance)
    {
        BossHealthWidgetInstance->RemoveFromParent();
        BossHealthWidgetInstance = nullptr;
    }

    if (APawn* PlayerPawn = PC->GetPawn())
    {
        PlayerPawn->DisableInput(PC);
    }

    FActorSpawnParameters SpawnParams;
    FVector CamLocation = GetActorLocation() + GetActorForwardVector() * 250.0f + FVector(0.0f, 0.0f, 50.0f);
    FRotator CamRotation = (GetActorLocation() - CamLocation).Rotation();

    CinematicCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), CamLocation, CamRotation, SpawnParams);

    if (CinematicCamera)
    {
        PC->SetViewTargetWithBlend(CinematicCamera, 1.5f, EViewTargetBlendFunction::VTBlend_Cubic);
    }

    GetWorld()->GetTimerManager().SetTimer(DeathSequenceTimer, this, &ADelveBoss::TriggerFadeOut, 3.0f, false);
}

void ADelveBoss::TriggerFadeOut()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 2.0f, FLinearColor::Black, false, true);
    }

    GetWorld()->GetTimerManager().SetTimer(FadeTimer, this, &ADelveBoss::ShowThankYouScreen, 2.0f, false);
}

void ADelveBoss::ShowThankYouScreen()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    if (DemoEndWidgetClass)
    {
        UUserWidget* EndWidget = CreateWidget<UUserWidget>(GetWorld(), DemoEndWidgetClass);
        if (EndWidget)
        {
            EndWidget->AddToViewport(9999); 
            
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(EndWidget->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }
}

// =========================================================
// [신규] 보스 전용 근접 공격 판정 (티라노 팔 해결!)
// =========================================================
void ADelveBoss::PerformMeleeDamageCheck()
{
    float BossRadius = GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleRadius() : 50.0f;
    FVector Start = GetActorLocation() + GetActorForwardVector() * (BossRadius + 50.0f);
    float HitRadius = AttackRange + 50.0f;

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(this, Start, HitRadius, ObjectTypes, nullptr, {}, OverlappedActors);

    for (AActor* HitActor : OverlappedActors)
    {
        // 1. 자기 자신과 동족(적)은 때리지 않음
        if (HitActor && HitActor != this && !HitActor->IsA(ADelveEnemy::StaticClass()))
        {
            // 2. [핵심] 명단에 이 타겟이 없다면? (아직 안 때렸다면?)
            if (!HitActorsThisAttack.Contains(HitActor))
            {
                FDamageEvent DamageEvent;
                HitActor->TakeDamage(Damage, DamageEvent, GetController(), this); // 데미지 1회 쾅!
                
                // 3. 때렸으니 명단에 올려서, 다음 Tick에서는 무시하도록 만듭니다!
                HitActorsThisAttack.Add(HitActor);
            }
        }
    }
}

// =========================================================
// [신규] 포효 시 화면 특수 효과 (카메라 쉐이크 + UI 오버레이)
// =========================================================
void ADelveBoss::PlayRoarScreenEffect()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    // 1. 카메라 지진 일으키기
    if (RoarCameraShakeClass)
    {
        PC->ClientStartCameraShake(RoarCameraShakeClass);
    }

    // 2. 특수 화면 효과(피눈물, 왜곡 등) 띄우기
    if (RoarScreenEffectWidgetClass)
    {
        UUserWidget* RoarWidget = CreateWidget<UUserWidget>(PC, RoarScreenEffectWidgetClass);
        if (RoarWidget)
        {
            // Z-Order를 -1로 주어서 체력바(기본 0)보다 뒤에, 게임 화면보다는 앞에 깔리게 합니다.
            RoarWidget->AddToViewport(-1); 
        }
    }
}

// =========================================================
// [신규] 포효 시 화면 특수 효과 (카메라 쉐이크 + UI 오버레이)
// =========================================================
void ADelveBoss::PlayFirstRoarScreenEffect()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    // 1. 카메라 지진 일으키기
    if (RoarCameraShakeClass)
    {
        PC->ClientStartCameraShake(FirstRoarCameraShakeClass);
    }
}