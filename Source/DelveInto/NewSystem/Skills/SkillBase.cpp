#include "SkillBase.h"
#include "TimerManager.h"
#include "NewSystem/DelveCharacter.h"

USkillBase::USkillBase()
{
    PrimaryComponentTick.bCanEverTick = true; // 쿨타임 및 차징 계산을 위해 필수
}

void USkillBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentState = ESkillState::Ready;
    CurrentComboCount = 0;
    CooldownRemaining = 0.0f;
}

float USkillBase::CalculateCooldownRemains(float DeltaTime)
{
    if (CooldownRemaining > 0.0f)
    {
        CooldownRemaining -= DeltaTime;
        if (CooldownRemaining <= 0.0f)
        {
            CooldownRemaining = 0.0f;
            if (CurrentState == ESkillState::Cooldown)
            {
                SetState(ESkillState::Ready);
            }
        }
    }
    return CooldownRemaining;
}

float USkillBase::CalculateComboResetRemains(float DeltaTime)
{
    if (CurrentComboCount > 0)
    {
        CurrentComboTimer -= DeltaTime;
        if (CurrentComboTimer <= 0.0f)
        {
            ResetCombo();
        }
    }
    return CurrentComboTimer;
}

float USkillBase::CalculateChargingProgress(float DeltaTime)
{
    // 3. 차징 진행
    if (CurrentState == ESkillState::PreCharging || CurrentState == ESkillState::Charging)
    {
        ChargingTime += DeltaTime;

        // PreCharging 종료 체크
        if (CurrentState == ESkillState::PreCharging)
        {
            float PreDuration = (PreChargingFlipbook) ? PreChargingFlipbook->GetTotalDuration() : 0.0f;
            if (ChargingTime >= PreDuration)
            {
                SetState(ESkillState::Charging);
            }
        }
        
        // 풀차징 도달 체크
        if (CurrentState == ESkillState::Charging && ChargingTime >= MaxChargeSeconds)
        {
            // MaxChargeSeconds가 0보다 클 때만 ChargedLoop로 넘어감
            if (MaxChargeSeconds > 0.0f)
            {
                SetState(ESkillState::ChargedLoop);
            }
        }
    }

    return ChargingTime;
}


void USkillBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    CalculateCooldownRemains(DeltaTime);

    CalculateComboResetRemains(DeltaTime);

    CalculateChargingProgress(DeltaTime);
}

void USkillBase::TryStartSkill()
{
    // 쿨타임이거나, 이미 행동 중(차징 완료 상태 제외)이면 실행 불가
    if (CooldownRemaining > 0.0f) return;
    if (CurrentState != ESkillState::Ready && CurrentState != ESkillState::ChargedLoop) return;

    // A. 차징 스킬: 차징 시작
    if (bIsChargingSkill)
    {
        ChargingTime = 0.0f;
        SetState(ESkillState::PreCharging);
    }
    // B. 즉발 스킬: 바로 실행
    else
    {
        ExecuteSkillLogic();
    }
}

void USkillBase::TryStopSkill()
{
    if (!bIsChargingSkill) return;

    // 차징 중에 뗐다면 발사
    if (CurrentState == ESkillState::PreCharging || 
        CurrentState == ESkillState::Charging || 
        CurrentState == ESkillState::ChargedLoop)
    {
        ExecuteSkillLogic();
    }
}

void USkillBase::ExecuteSkillLogic()
{
    SetState(ESkillState::Casting);

    // 1. 데미지 계산 및 알림
    float Dmg = CalculateDamage();
    OnSkillExecuted.Broadcast(this, Dmg);

    // 2. 콤보 처리
    if (bHasCombo)
    {
        CurrentComboCount++;
        CurrentComboTimer = ComboResetTime; // 타이머 리셋
        
        // 최대 콤보 초과 시 0으로 (또는 기획에 따라 유지)
        if (CurrentComboCount >= MaxComboCount)
        {
            // 여기서는 콤보 인덱스로 쓰기 위해 나머지 연산
            // (예: 3콤보 -> 0, 1, 2 순환)
        }
    }

    // 3. 애니메이션 재생 (콤보 인덱스 적용)
    UPaperFlipbook* AnimToPlay = CastingFlipbook;
    
    // 콤보용 배열이 있다면 해당 인덱스 재생
    int32 AnimIndex = (CurrentComboCount > 0) ? (CurrentComboCount - 1) % FMath::Max(1, ComboFlipbooks.Num() + 1) : 0;
    
    if (ComboFlipbooks.IsValidIndex(AnimIndex))
    {
        AnimToPlay = ComboFlipbooks[AnimIndex];
    }
    // 기본값 CastingFlipbook 사용
    else if (CastingFlipbook) 
    {
        AnimToPlay = CastingFlipbook;
    }

    float Duration = (AnimToPlay) ? AnimToPlay->GetTotalDuration() : 0.5f;
    RequestPlayAnimation(AnimToPlay, false);

    // 4. 애니메이션 종료 후 후딜레이/쿨타임 처리
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, [this]()
    {
        if (PostCastingFlipbook)
        {
            SetState(ESkillState::PostCasting);
            
            // 후딜레이 후 쿨타임
            float PostDuration = PostCastingFlipbook->GetTotalDuration();
            FTimerHandle PostHandle;
            GetWorld()->GetTimerManager().SetTimer(PostHandle, [this](){
                ApplyCooldown();
            }, PostDuration, false);
        }
        else
        {
            ApplyCooldown();
        }

        // 콤보 사이클이 돌았다면 초기화 (예: 3타 때리고 나면 다시 1타부터)
        if (bHasCombo && CurrentComboCount >= MaxComboCount)
        {
            ResetCombo();
        }

    }, Duration, false);
}

void USkillBase::ApplyCooldown() // 내부 헬퍼
{
    if (bHasCooldown)
    {
        CooldownRemaining = CooldownSeconds;
        SetState(ESkillState::Cooldown);
    }
    else
    {
        SetState(ESkillState::Ready);
    }
}

float USkillBase::CalculateDamage() const
{
    float Dmg = BaseDamage;

    // 1. 난수 적용
    if (bHasVariableDamage)
    {
        Dmg = FMath::RandRange(BaseDamage, MaxDamage);
    }

    // 2. 차징 보정 (예시 로직: 풀차징 시 1.5배)
    if (bIsChargingSkill && MaxChargeSeconds > 0.0f)
    {
        float Ratio = FMath::Clamp(ChargingTime / MaxChargeSeconds, 0.0f, 1.0f);
        Dmg *= (1.0f + (Ratio * 0.5f)); 
    }

    return Dmg;
}

void USkillBase::SetState(ESkillState NewState)
{
    CurrentState = NewState;

    UPaperFlipbook* Anim = nullptr;
    bool bLoop = false;

    switch (NewState)
    {
        case ESkillState::PreCharging:
            Anim = PreChargingFlipbook; bLoop = false;
            break;
        case ESkillState::Charging:
            Anim = ChargingFlipbook; bLoop = true;
            break;
        case ESkillState::ChargedLoop:
            Anim = PostChargingFlipbook; bLoop = true;
            break;
        case ESkillState::PostCasting:
            Anim = PostCastingFlipbook; bLoop = false;
            break;
        case ESkillState::Casting:
            // Casting은 ExecuteSkillLogic에서 별도 처리 (콤보 때문에)
            return; 
    }

    if (Anim)
    {
        RequestPlayAnimation(Anim, bLoop);
    }
}

void USkillBase::ResetCombo()
{
    CurrentComboCount = 0;
    CurrentComboTimer = 0.0f;
}

void USkillBase::CancelSkill()
{
    ResetCombo();
    ChargingTime = 0.0f;
    SetState(ESkillState::Ready);
}

void USkillBase::RequestPlayAnimation(UPaperFlipbook* Flipbook, bool bLoop)
{
    if (ADelveCharacter* OwnerChar = Cast<ADelveCharacter>(GetOwner()))
    {
        OwnerChar->UpdateWeaponUI(Flipbook, bLoop);
    }
}