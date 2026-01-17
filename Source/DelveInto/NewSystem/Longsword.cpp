#include "Longsword.h"

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
    ApplyDamageSphericalCone(Damage, 250.0f, 60.0f);

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

void ALongsword::TrySecondaryAttack(bool bIsHolding)
{
    // 차징 구현 (간소화: 애니메이션은 Map의 Secondary 슬롯 사용)
    if (bIsHolding)
    {
        if (ChargeStartTime <= 0.0f)
        {
            ChargeStartTime = GetWorld()->GetTimeSeconds();
            // 차징 시작 모션 (Secondary 슬롯에 등록된 경우)
            PlayAnimationBySlot(EWeaponSkillSlot::Secondary); 
        }
    }
    else
    {
        if (ChargeStartTime > 0.0f)
        {
            float HoldTime = GetWorld()->GetTimeSeconds() - ChargeStartTime;
            float Alpha = FMath::Clamp(HoldTime / MaxChargeTime, 0.0f, 1.0f);
            float FinalDamage = FMath::Lerp(10.0f, 20.0f, Alpha);

            UE_LOG(LogTemp, Log, TEXT("Charged Attack: %f"), FinalDamage);
            
            // 검기 발사 (더 먼 거리 판정)
            ApplyDamageSphere(FinalDamage, 100.0f, FVector(400.0f, 0.0f, 0.0f)); 

            ChargeStartTime = 0.0f;
            ReturnToIdle(); // 발사 후 즉시 복귀
        }
    }
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