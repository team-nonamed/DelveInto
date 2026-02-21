#include "SkillBase.h"
#include "TimerManager.h"
#include "EditorState/EditorState.h"
#include "Kismet/GameplayStatics.h"
#include "NewSystem/Utils/FlipbookUtil.h"


DEFINE_LOG_CATEGORY(LogSkill);

USkillBase::USkillBase()
{
    // Cooldown Time 및 Charging Time 계산을 위해 활성화
    PrimaryComponentTick.bCanEverTick = true;
}

void USkillBase::BeginPlay()
{
    Super::BeginPlay();

    ResetSkill();

    MaxChargingSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(ChargingFlipbook, CastSpeed);
}

void USkillBase::ResetSkill()
{
    auto TimerManager = &GetWorld()->GetTimerManager();
    TimerManager->ClearAllTimersForObject(this); // 한 번에 클리어

    bIsActivated = false;
    bIsKeyPressed = false;

    bIsActivated = false;
    bIsKeyPressed = false;
    
    CurrentState = ESkillState::Idle;
    ChargingSeconds = 0.0f;

    if (bHasCombo)
    {
        ComboDurationSeconds = 0.0f;
    }

    if (bHasCooldown)
    {
        CooldownSeconds = 0.0f;
    }
}

void USkillBase::DeactivateSkill()
{
    auto TimerManager = &GetWorld()->GetTimerManager();

    TimerManager->ClearAllTimersForObject(this);
    
    bIsActivated = false;
    bIsKeyPressed = false;
    
    CurrentState = ESkillState::Cooldown;
    ChargingSeconds = 0.0f;

    UE_LOG(LogSkill, Display, TEXT("%s을 종료합니다. Combo:%d (%f 초), Cooldown: %f"), *SkillName.ToString(), CurrentComboCount, ComboDurationSeconds, CooldownSeconds)

    if (bHasCombo && ComboDurationSeconds > 0.0f)
    {
        CooldownSeconds = 0.0f;
        CurrentState = ESkillState::Idle;
    }
    else if (bHasCooldown)
    {
        CooldownSeconds = MaxCooldownSeconds;
    }
    else
    {
        CooldownSeconds = 0.0f;
        CurrentState = ESkillState::Idle;
    }

    OnSkillDeactivated.Broadcast(this);
}

bool USkillBase::HandleKeyPressed(float InCastSpeed)
{
    bIsKeyPressed = true;

    // Cooldown을 모두 소모하지 못한 경우 실행이 절대로 안됨
    if (CooldownSeconds > 0.0f || CurrentState == ESkillState::Cooldown)
    {
        UE_LOG(LogSkill, Display, TEXT("%s는 쿨타임(%f) 중입니다."), *SkillName.ToString(), CooldownSeconds);
        return false;
    }

    // Idle, Charging, PostCharging 외의 상태라면 입력 무시
    if (CurrentState != ESkillState::Idle && CurrentState != ESkillState::Charging && CurrentState != ESkillState::PostCharging)
    {
        UE_LOG(LogSkill, Display, TEXT("%s은 현재 Casting 중입니다."), *SkillName.ToString());
        return false;
    }

    if (CurrentState == ESkillState::Idle)
    {
        UE_LOG(LogSkill, Display, TEXT("%s은 현재 Idle 상태입니다. 활성화를 시작합니다."), *SkillName.ToString());
        ActivateSkill();
        CastSpeed = InCastSpeed;
        return true;
    }
    
    if (ExecutionType == ESkillExecutionType::PressAfterCharging)
    {
        // 이미 풀 차징일 경우 모든 경우에서 발사해도 됨
        if (CurrentState == ESkillState::PostCharging)
        {
            GetWorld()->GetTimerManager().ClearTimer(PostChargingExpirationHandle);
            HandlePreCasting();
            return true;
        }

        // Charging이 다 안되어도 발사 가능한 Skill의 경우 PreCast로 넘김
        if (CurrentState == ESkillState::Charging && bCanFireEarly)
        {
            GetWorld()->GetTimerManager().ClearTimer(ChargingExpirationHandle);
            HandlePreCasting();
            return true;
        }
    }

    return false;
}

bool USkillBase::HandleKeyReleased()
{
    if (ExecutionType != ESkillExecutionType::ReleaseAfterHold)
    {
        return false;
    }
    
    if ((CurrentState == ESkillState::PostCharging || CurrentState == ESkillState::Charging) && bIsActivated)
    {
        GetWorld()->GetTimerManager().ClearTimer(PostChargingExpirationHandle);
        GetWorld()->GetTimerManager().ClearTimer(ChargingExpirationHandle);
        HandlePreCasting();

        return true;
    }

    return false;
}

void USkillBase::ActivateSkill()
{
    OnSkillActivated.Broadcast(this);

    bIsActivated = true;
    if (!bIsChargingSkill)
    {
        HandlePreCasting();
    }
    else
    {
        HandlePreCharging();
    }
}

void USkillBase::HandlePreCharging()
{
    // 즉발 스킬의 경우 바로 PreCasting으로 넘김
    if (!bIsChargingSkill)
    {
        return HandlePreCasting();
    }

    if (!PreChargingFlipbook)
    {
        UE_LOG(LogSkill, Warning, TEXT("%s에 Pre Charging Flipbook이 없습니다. 생략합니다."), *SkillName.ToString())
        HandleCharging();
        return;
    }
    
    float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(PreChargingFlipbook, CastSpeed);

    if (FlipbookDurationSeconds <= 0.0f)
    {
        HandleCharging();
    }
    
    CurrentState = ESkillState::PreCharging;
    
    OnPreCharging.Broadcast(CurrentState, PreChargingFlipbook, false);
    
    GetWorld()->GetTimerManager().
        SetTimer(PreChargingExpirationHandle, this, &USkillBase::HandleCharging, FlipbookDurationSeconds, false);

    CooldownSeconds = MaxCooldownSeconds;
}

void USkillBase::HandleCharging()
{
    if (!ChargingFlipbook)
    {
        UE_LOG(LogSkill, Display, TEXT("%s에 할당된 유효한 Charging Flipbook이 존재하지 않습니다. 생략합니다."), *SkillName.ToString());
        HandlePostCharging();
        return;
    }
    
    MaxChargingSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(ChargingFlipbook, CastSpeed);

    if (MaxChargingSeconds <= 0.0f)
    {
        UE_LOG(LogSkill, Display, TEXT("%s에 할당된 유효한 Charging Flipbook이 존재하지 않습니다. 생략합니다."), *SkillName.ToString());
        HandlePostCharging();
        return;
    }
     
    CurrentState = ESkillState::Charging;
    
    OnCharging.Broadcast(CurrentState, ChargingFlipbook, false);
    
}

void USkillBase::HandlePostCharging()
{
    if (!PostChargingFlipbook)
    {
        UE_LOG(LogSkill, Display, TEXT("%s에는 Post Charging Flipbook이 존재하지 않습니다. 생략합니다."), *SkillName.ToString())

        if (ExecutionType == ESkillExecutionType::PressAfterCharging)
        {
            HandlePostCasting(false);
            return;
        }
        
        HandlePreCasting();
    }

    float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(PostChargingFlipbook, CastSpeed);
    if (FlipbookDurationSeconds <= 0.0f)
    {
        UE_LOG(LogSkill, Display, TEXT("%s에는 유효한 Post Charging Flipbook이 존재하지 않습니다. 생략합니다."), *SkillName.ToString())

        if (ExecutionType == ESkillExecutionType::PressAfterCharging)
        {
            HandlePostCasting(false);
            return;
        }
        HandlePreCasting();
    }

    CurrentState = ESkillState::PostCharging;

    

    if (bIsAutoCasting)
    {
        // 모두 다 소진되면 Skill의 사용 가능 여부가 만료된 것으로 간주
        // 시전 실패로 간주
        if (ExecutionType == ESkillExecutionType::PressAfterCharging)
        {
            OnPostCharging.Broadcast(CurrentState, PostChargingFlipbook, false);
            
            FTimerDelegate TimerDelegate;

            TimerDelegate.BindUObject(this, &USkillBase::HandlePostCasting, false);
            
            GetWorld()->GetTimerManager().SetTimer(
                PostChargingExpirationHandle,
                TimerDelegate,
                FlipbookDurationSeconds,
                false
                );
            return;
        }

        // 모두 다 시간이 소진되면 자동 사용
        if (ExecutionType == ESkillExecutionType::ReleaseAfterHold)
        {
            OnPostCharging.Broadcast(CurrentState, PostChargingFlipbook, false);
            
            GetWorld()->GetTimerManager().SetTimer(
                PostChargingExpirationHandle,
                this,
                &USkillBase::HandlePreCasting,
                FlipbookDurationSeconds,
                false
                );
            return;
        }

        OnPostCharging.Broadcast(CurrentState, PostChargingFlipbook, true);
        return;
        
    }

    OnPostCharging.Broadcast(CurrentState, PostChargingFlipbook, true);
}

void USkillBase::HandlePreCasting()
{

    if (!bHasCombo)
    {
        if (!PreCastingFlipbook)
        {
            UE_LOG(LogSkill, Display, TEXT("%s는 Pre Casting Flipbook이 존재하지 않습니다. 생략합니다."), *SkillName.ToString())
            HandleCasting();
        }
        else
        {
            CurrentState = ESkillState::PreCasting;
            
            float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(PreCastingFlipbook, CastSpeed);

            if (FlipbookDurationSeconds <= 0.0f)
            {
                return HandleCasting();
            }
            
            OnPreCasting.Broadcast(CurrentState, PreCastingFlipbook, false);
            GetWorld()->GetTimerManager().
                SetTimer(PreCastingExpirationHandle, this, &USkillBase::HandleCasting, FlipbookDurationSeconds, false);
        }
    }
    else
    {
        UPaperFlipbook* Flipbook = GetFlipbookForCombo(PreCastingFlipbooks, PreCastingFlipbook);
        if (!Flipbook)
        {
            UE_LOG(LogSkill, Display, TEXT("%s는 Pre Casting Flipbook이 존재하지 않습니다. 생략합니다."), *SkillName.ToString())
            HandleCasting();
            return;
        }

        float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(Flipbook, CastSpeed);

        if (FlipbookDurationSeconds <= 0.0f)
        {
            return HandleCasting();
        }

        CurrentState = ESkillState::PreCasting;
        
        OnPreCasting.Broadcast(CurrentState, Flipbook, false);
        GetWorld()->GetTimerManager().
            SetTimer(PreCastingExpirationHandle, this, &USkillBase::HandleCasting, FlipbookDurationSeconds, false);
    }
}

void USkillBase::HandleCasting()
{
    if (!bHasCombo)
    {
        if (!CastingFlipbook)
        {
            UE_LOG(LogSkill, Error, TEXT("%s는 Casting Flipbook이 존재하지 않습니다."), *SkillName.ToString());
            return DeactivateSkill();
        }

        CurrentState = ESkillState::Casting;
        float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(CastingFlipbook, CastSpeed);

        if (FlipbookDurationSeconds <= 0.0f)
        {
            UE_LOG(LogSkill, Error, TEXT("%s는 유효한 Casting Flipbook이 존재하지 않습니다."), *SkillName.ToString());
            return DeactivateSkill();
        }

        OnCasting.Broadcast(CurrentState, CastingFlipbook, false);

        bool IsSuccess = ExecuteSkillLogic();
        
        if (IsSuccess)
        {
            UGameplayStatics::PlaySoundAtLocation(this, CastButHurtSuccessSound, GetOwner()->GetActorLocation());
        }
        else
        {
            UGameplayStatics::PlaySoundAtLocation(this, CastButHurtFailedSound, GetOwner()->GetActorLocation());
        }

        if (!bEndCastingManually)
        {
            FTimerDelegate TimerDelegate;
            TimerDelegate.BindUObject(this, &USkillBase::HandlePostCasting, IsSuccess);
    
            GetWorld()->GetTimerManager().SetTimer(CastingExpirationHandle, TimerDelegate, FlipbookDurationSeconds, false);
        }
    }
    else
    {
        UPaperFlipbook* Flipbook = GetFlipbookForCombo(CastingFlipbooks, CastingFlipbook);
        if (!Flipbook)
        {
            UE_LOG(LogSkill, Error, TEXT("%s는 Casting Flipbook이 존재하지 않습니다."), *SkillName.ToString());
            HandlePostCasting(false);
            return;
        }

        float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(Flipbook, CastSpeed);

        if (FlipbookDurationSeconds <= 0.0f)
        {
            UE_LOG(LogSkill, Error, TEXT("%s는 유효한 Casting Flipbook이 존재하지 않습니다."), *SkillName.ToString());
            return HandlePostCasting(false);
        }

        CurrentState = ESkillState::Casting;
        
        bool IsSuccess = ExecuteSkillLogic();
        
        if (IsSuccess)
        {
            UGameplayStatics::PlaySoundAtLocation(this, CastButHurtSuccessSound, GetOwner()->GetActorLocation());
        }
        else
        {
            UGameplayStatics::PlaySoundAtLocation(this, CastButHurtFailedSound, GetOwner()->GetActorLocation());
        }

        OnCasting.Broadcast(CurrentState, Flipbook, false);
        
        if (!bEndCastingManually)
        {
            FTimerDelegate TimerDelegate;
            TimerDelegate.BindUObject(this, &USkillBase::HandlePostCasting, IsSuccess);
    
            GetWorld()->GetTimerManager().SetTimer(CastingExpirationHandle, TimerDelegate, FlipbookDurationSeconds, false);
        }
    }
}

void USkillBase::HandlePostCasting(bool IsSuccess)
{
    if (!bHasCombo)
    {
        if (!PostCastingFlipbook)
        {
            UE_LOG(LogSkill, Error, TEXT("%s는 Post Casting Flipbook이 존재하지 않습니다. 생략하고 스킬을 종료합니다."), *SkillName.ToString());
            DeactivateSkill();
        }
        else
        {
            float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(PostCastingFlipbook, CastSpeed);

            if (FlipbookDurationSeconds <= 0.0f)
            {
                UE_LOG(LogSkill, Error, TEXT("%s는 유효한 Post Casting Flipbook이 존재하지 않습니다. 생략하고 스킬을 종료합니다."), *SkillName.ToString());
                return DeactivateSkill();
            }

            CurrentState = ESkillState::PostCasting;
            
            OnPostCasting.Broadcast(CurrentState, PostCastingFlipbook, false);
            
            GetWorld()->GetTimerManager().
                SetTimer(PostCastingExpirationHandle, this, &USkillBase::DeactivateSkill, FlipbookDurationSeconds, false);

            CooldownSeconds = MaxCooldownSeconds;
            bIsActivated = false;
        }
    }
    else
    {
        UPaperFlipbook* Flipbook = GetFlipbookForCombo(PostCastingFlipbooks, PostCastingFlipbook);
        if (!Flipbook)
        {
            UE_LOG(LogSkill, Error, TEXT("%s는 Post Casting Flipbook이 존재하지 않습니다. 생략하고 스킬을 종료합니다."), *SkillName.ToString());

            if (IsSuccess)
            {
                CurrentComboCount++;

                if (CurrentComboCount >= MaxComboCount)
                {
                    switch (ComboTerminationType){
                    case EComboTerminationType::Unavailable:
                        // 루프형이 아니면 여기서 콤보 종료 (타이머 0으로 만들어서 Deactivate에서 쿨타임 먹게 함)
                        ComboDurationSeconds = 0.0f;
                        CurrentComboCount = 0;
                        break;
                    case EComboTerminationType::LastContinue:
                        CurrentComboCount = MaxComboCount - 1;
                        ComboDurationSeconds = MaxComboDurationSeconds;
                        break;
                    case EComboTerminationType::Loop:
                        CurrentComboCount = 0;
                        ComboDurationSeconds = MaxComboDurationSeconds;
                        break;
                    }
                }
                else
                {
                    ComboDurationSeconds = MaxComboDurationSeconds;
                }
            }
            
            DeactivateSkill();
            return;
        }

        float FlipbookDurationSeconds = UFlipbookUtil::GetAdjustedFlipbookDuration(Flipbook, CastSpeed);

        if (FlipbookDurationSeconds <= 0.0f)
        {
            UE_LOG(LogSkill, Error, TEXT("%s는 유효한 Post Casting Flipbook이 존재하지 않습니다. 생략하고 스킬을 종료합니다."), *SkillName.ToString());

            if (IsSuccess)
            {
                CurrentComboCount++;

                if (CurrentComboCount >= MaxComboCount)
                {
                    switch (ComboTerminationType){
                    case EComboTerminationType::Unavailable:
                        // 루프형이 아니면 여기서 콤보 종료 (타이머 0으로 만들어서 Deactivate에서 쿨타임 먹게 함)
                        ComboDurationSeconds = 0.0f;
                        CurrentComboCount = 0;
                        break;
                    case EComboTerminationType::LastContinue:
                        CurrentComboCount = MaxComboCount - 1;
                        ComboDurationSeconds = MaxComboDurationSeconds;
                        break;
                    case EComboTerminationType::Loop:
                        CurrentComboCount = 0;
                        ComboDurationSeconds = MaxComboDurationSeconds;
                        break;
                    }
                }
                else
                {
                    ComboDurationSeconds = MaxComboDurationSeconds;
                }
            }
            
            return DeactivateSkill();
        }

        CurrentState = ESkillState::PostCasting;
            
        OnPostCasting.Broadcast(CurrentState, Flipbook, false);
        GetWorld()->GetTimerManager().
            SetTimer(PostCastingExpirationHandle, this, &USkillBase::DeactivateSkill, FlipbookDurationSeconds, false);

        CurrentComboCount++;

        if (CurrentComboCount >= MaxComboCount)
        {
            switch (ComboTerminationType){
            case EComboTerminationType::Unavailable:
                // 루프형이 아니면 여기서 콤보 종료 (타이머 0으로 만들어서 Deactivate에서 쿨타임 먹게 함)
                ComboDurationSeconds = 0.0f;
                CurrentComboCount = 0;
                break;
            case EComboTerminationType::LastContinue:
                CurrentComboCount = MaxComboCount - 1;
                ComboDurationSeconds = MaxComboDurationSeconds;
                break;
            case EComboTerminationType::Loop:
                CurrentComboCount = 0;
                ComboDurationSeconds = MaxComboDurationSeconds;
                break;
            }
        }
        else
        {
            ComboDurationSeconds = MaxComboDurationSeconds;
        }
    }
}


float USkillBase::CalculateCooldownSeconds(float DeltaTime)
{
    if (CooldownSeconds > 0.0f)
    {
        CooldownSeconds -= DeltaTime;
        if (CooldownSeconds <= 0.0f)
        {
            CooldownSeconds = 0.0f;
            if (CurrentState == ESkillState::Cooldown)
            {
                CurrentState = ESkillState::Idle;
            }
        }
    }
    return CooldownSeconds;
}

float USkillBase::CalculateComboDurationSeconds(float DeltaTime)
{
    if (!bHasCombo)
    {
        return 0.0f;
    }

    if (CurrentComboCount == 0)
    {
        return 0.0f;
    }

    if (ComboDurationSeconds <= 0.0f)
    {
        ResetCombo();

        if (bHasCooldown && CooldownSeconds <= 0.0f)
        {
            CooldownSeconds = MaxCooldownSeconds;
            // 필요하다면 상태 갱신 등을 위해 DeactivateSkill 호출
            // DeactivateSkill(); 
        }
        
        return 0.0f;
    }
    
    if (CurrentComboCount > 0 && (CurrentState == ESkillState::Idle || CurrentState == ESkillState::Cooldown || CurrentState == ESkillState::PostCasting))
    {
        ComboDurationSeconds -= DeltaTime;
        if (ComboDurationSeconds <= 0.0f)
        {
            ResetCombo();
            if (bHasCooldown && CooldownSeconds <= 0.0f)
            {
                CooldownSeconds = MaxCooldownSeconds;
                // 필요하다면 상태 갱신 등을 위해 DeactivateSkill 호출
                // DeactivateSkill(); 
            }
        }
    }
    return ComboDurationSeconds;
}

float USkillBase::CalculateChargingSeconds(float DeltaTime)
{
    if (CurrentState == ESkillState::Charging)
    {
        ChargingSeconds += DeltaTime;
        if (ChargingSeconds >= MaxChargingSeconds)
        {
            ChargingSeconds = MaxChargingSeconds;
            HandlePostCharging(); // Tick에서 상태 전이
        }
    }
    return ChargingSeconds;
}


void USkillBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    CalculateCooldownSeconds(DeltaTime);

    CalculateComboDurationSeconds(DeltaTime);

    CalculateChargingSeconds(DeltaTime);
}

// /**
//  * @deprecated 
//  */
// void USkillBase::HandleKeyReleased()
// {
//     if (!bIsChargingSkill) return;
//
//     // 차징 중에 뗐다면 발사
//     if (CurrentState == ESkillState::PreCharging || 
//         CurrentState == ESkillState::Charging || 
//         CurrentState == ESkillState::ChargedLoop)
//     {
//         ExecuteSkillLogic();
//     }
// }

float USkillBase::CalculateDamage() const
{
    float Dmg = BaseDamage;

    // 1. 난수 적용
    if (bHasVariableDamage)
    {
        Dmg = FMath::RandRange(BaseDamage, MaxDamage);
    }

    // 2. 차징 보정 (예시 로직: 풀차징 시 1.5배)
    if (bIsChargingSkill && MaxChargingSeconds > 0.0f)
    {
        float Ratio = FMath::Clamp(ChargingSeconds / MaxChargingSeconds, 0.0f, 1.0f);
        Dmg *= (1.0f + (Ratio * 0.5f)); 
    }

    return Dmg;
}

void USkillBase::ResetCombo()
{
    CurrentComboCount = 0;
    ComboDurationSeconds = 0.0f;
}

UPaperFlipbook* USkillBase::GetFlipbookForCombo(const TArray<UPaperFlipbook*>& ComboFlipbooks, UPaperFlipbook* DefaultFlipbook) const
{
    if (bHasCombo && ComboFlipbooks.IsValidIndex(CurrentComboCount))
    {
        return ComboFlipbooks[CurrentComboCount];
    }
    return DefaultFlipbook;
}

bool USkillBase::ExecuteSkillLogic_Implementation()
{
    // 기본 구현: 로그 출력
    // 자식 클래스에서 Super::ExecuteSkillLogic_Implementation()을 호출할 필요는 없음 (선택)
    UE_LOG(LogSkill, Error, TEXT("USkillBase::ExecuteSkillLogic을 재정의하세요."))

    return false;
    // 여기서 투사체를 스폰하거나, 범위 공격 판정을 하거나, 버프를 줍니다.
}