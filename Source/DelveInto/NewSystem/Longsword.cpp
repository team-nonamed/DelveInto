#include "Longsword.h"

#include "DelveCharacter.h"
#include "DelveProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

void ALongsword::TryPrimaryAttack()
{
    if (bIsAttacking) return;

    // 1. 콤보 애니메이션 선택 및 재생
    UPaperFlipbook* CurrentAnim = nullptr;
    if (PrimaryComboAnims.IsValidIndex(CurrentComboIndex))
    {
        CurrentAnim = PrimaryComboAnims[CurrentComboIndex];
    }
    
    // 애니메이션 재생 (없으면 0.0f 반환)
    float AnimDuration = PlayAnimationDirectly(CurrentAnim);
    if (AnimDuration <= 0.0f) AnimDuration = 0.5f; // 안전장치

    // 2. 데미지 계산 (10 -> 10 -> 15)
    float Damage = 10.0f;
    if (CurrentComboIndex == 2) Damage = 15.0f;

    // [호출] 
    // Radius: 200 (사거리)
    // HalfAngle: 60 (좌우 120도)
    // HalfHeight: AttackHeight (플레이어 키만큼 위아래 커버)
    ApplyDamageSphericalCone(Damage, 400.0f, 15.0f);

    // 4. 콤보 인덱스 증가 및 초기화 타이머
    CurrentComboIndex = (CurrentComboIndex + 1) % PrimaryComboAnims.Num(); // 배열 길이만큼 순환
    GetWorld()->GetTimerManager().SetTimer(ComboResetTimer, this, &ALongsword::ResetCombo, 1.5f, false);

    // 5. 공격 종료 후 Idle 복귀 예약
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, this, &AWeaponBase::ReturnToIdle, AnimDuration, false);
}

void ALongsword::ResetCombo()
{
    CurrentComboIndex = 0;
    // (선택) UI 콤보 초기화 등 처리
}

void ALongsword::TrySecondaryAttack(bool bIsPressed)
{
    // 1. [누름] 준비 동작 시작
    if (bIsPressed)
    {
        if (bIsCharging) return; // 이미 차징 중이면 무시

        bIsCharging = true;
        ChargeStartTime = GetWorld()->GetTimeSeconds();

        // 1단계: Prepare 애니메이션 재생 (1회성)
        if (MyOwnerCharacter && AlterPrepareFlipbook)
        {
            MyOwnerCharacter->UpdateWeaponUI(AlterPrepareFlipbook, false); // false = 반복 안 함

            // Prepare 애니메이션 길이만큼 대기 후 -> Charge 루프로 전환
            float PrepareDuration = AlterPrepareFlipbook->GetTotalDuration();
            
            // 타이머 설정: 애니메이션이 끝날 때 StartChargeLoop 함수 실행
            GetWorld()->GetTimerManager().SetTimer(PrepareTimerHandle, this, &ALongsword::StartChargeLoop, PrepareDuration, false);
        }
        else
        {
            // 만약 Prepare 애니메이션이 없으면 바로 루프 시작
            StartChargeLoop();
        }
    }
    // 2. [뗌] 발사
    else
    {
        if (!bIsCharging) return;

        bIsCharging = false;

        // 혹시 Prepare 도중에 뗐다면, Charge로 넘어가는 타이머 취소
        GetWorld()->GetTimerManager().ClearTimer(PrepareTimerHandle);

        // 차징 비율 계산
        float HeldTime = GetWorld()->GetTimeSeconds() - ChargeStartTime;
        float ChargeRatio = FMath::Clamp(HeldTime / MaxChargeTime, 0.0f, 1.0f);

        // 발사 로직 실행
        FireSwordWave(ChargeRatio);
    }
}

// 타이머에 의해 호출됨: Prepare가 끝났고 아직 누르고 있다면 실행
void ALongsword::StartChargeLoop()
{
    // 버튼을 떼서 bIsCharging이 false가 되었다면 실행 안 함
    if (bIsCharging && MyOwnerCharacter && AlterChargeFlipbook)
    {
        // 2단계: Charge 애니메이션 재생 (계속 반복)
        MyOwnerCharacter->UpdateWeaponUI(AlterChargeFlipbook, true); // true = 반복 함
    }
}

void ALongsword::FireSwordWave(float ChargeRatio)
{
    // 3단계: Cast 애니메이션 재생 (1회성)
    float CastDuration = 0.5f; // 기본값
    if (MyOwnerCharacter && AlterCastFlipbook)
    {
        MyOwnerCharacter->UpdateWeaponUI(AlterCastFlipbook, false); // false = 반복 안 함
        CastDuration = AlterCastFlipbook->GetTotalDuration();
    }

    // 투사체 생성 (기존 로직 유지)
    if (SwordWaveClass && MyOwnerCharacter)
    {
        UCameraComponent* Camera = MyOwnerCharacter->FirstPersonCamera;
        FVector SpawnLoc = Camera->GetComponentLocation() + (Camera->GetForwardVector() * 50.0f);
        FRotator SpawnRot = Camera->GetComponentRotation();

        FActorSpawnParameters Params;
        Params.Owner = MyOwnerCharacter;
        Params.Instigator = MyOwnerCharacter;

        ADelveProjectile* Wave = GetWorld()->SpawnActor<ADelveProjectile>(SwordWaveClass, SpawnLoc, SpawnRot, Params);
        if (Wave)
        {
            Wave->InitializeChargeStats(ChargeRatio);
        }
    }

    // 초기화
    ChargeStartTime = 0.0f;

    // 4단계: Cast 동작이 끝나면 Idle로 돌아오기 위해 타이머 설정
    // (기존 ReturnToIdle 대신 타이머를 사용해야 애니메이션이 안 끊김)
    FTimerHandle IdleReturnTimer;
    GetWorld()->GetTimerManager().SetTimer(IdleReturnTimer, this, &AWeaponBase::ReturnToIdle, CastDuration, false);
}

void ALongsword::TrySkillQ()
{
    if (IsOnCooldown(EWeaponSkillSlot::SkillQ) || bIsAttacking) return;

    // 1. 애니메이션 재생 (Map에서 SkillQ 찾아서 재생)
    float Duration = PlayAnimationBySlot(EWeaponSkillSlot::SkillQ);
    if (Duration <= 0.0f) Duration = 0.8f;

    // 2. 쿨타임
    SetCooldown(EWeaponSkillSlot::SkillQ, 8.0f);

    // 3. 로직 (돌진 찌르기)
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        FVector LaunchDir = OwnerCharacter->GetActorForwardVector() * 1500.0f;
        OwnerCharacter->LaunchCharacter(LaunchDir, true, false);
        
        // 돌진 판정
        ApplyDamageSphere(30.0f, 100.0f, FVector(200.0f, 0.0f, 0.0f));
    }

    // 4. 복귀 예약
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, this, &AWeaponBase::ReturnToIdle, Duration, false);
}

void ALongsword::TrySkillE()
{
    if (IsOnCooldown(EWeaponSkillSlot::SkillE) || bIsAttacking) return;

    // 1. 애니메이션 재생
    float Duration = PlayAnimationBySlot(EWeaponSkillSlot::SkillE);
    if (Duration <= 0.0f) Duration = 1.0f;

    // 2. 쿨타임
    SetCooldown(EWeaponSkillSlot::SkillE, 10.0f);

    // 3. 로직 (회전 베기 - 주변 넓은 범위)
    ApplyDamageSphere(20.0f, 300.0f, FVector::ZeroVector);

    // 4. 복귀 예약
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, this, &AWeaponBase::ReturnToIdle, Duration, false);
}