#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbook.h"
#include "SkillState.h"
#include "Components/ActorComponent.h"
#include "SkillBase.generated.h"

// [신규] 스킬 실행 결과를 알리기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillExecuted, USkillBase*, SkillInst, float, Damage);

UCLASS(Abstract, Blueprintable, ClassGroup=(Skill), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkillBase : public UActorComponent
{
    GENERATED_BODY()

#pragma region Inner Events
public:
    // Sets default values for this component's properties
    USkillBase();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Properties
public:

    // Skill의 기본 정보(이름, 설명, 아이콘)
    #pragma region Skill Info
       /**
        * Skill의 표시 명칭 (UI 등에서 사용)
        */
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Info")
       FText SkillName;

       /**
        * Skill의 상세 설명 (툴팁 등에서 사용)
        */
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Info")
       FText SkillDescription;

       /**
        * Skill의 아이콘 (UI 등에서 사용)
        */
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Info")
       UTexture2D* SkillIcon;
    #pragma endregion

    // Skill의 설정 값(쿨다운, 콤보 등)
    #pragma region Skill Config
       
       // [추가] 기능 토글 (EditCondition용 변수)
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
       bool bIsChargingSkill = false; // 차징 스킬인가?

       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
       bool bHasCombo = false;        // 콤보 스킬인가?

       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
       bool bHasVariableDamage = false; // 난수 데미지인가?

       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
       bool bHasCooldown = true;      // 쿨타임이 있는가?
       
       // -----------------------------------------------------------------

       // Skill의 기본 피해량
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(ClampMin = "0.0"))
       float BaseDamage = 10.0f;

       // Skill의 최대 피해량 (난수 데미지일 때 사용)
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(ClampMin = "0.0", EditCondition = "bHasVariableDamage"))
       float MaxDamage = 20.0f;
    
       // Skill의 쿨타임 (초)
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bHasCooldown", ClampMin = "0.0"))
       float CooldownSeconds = 1.0f;
       
       // Skill의 최대 Charging 시간 (차징 스킬일 때 사용)
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bIsChargingSkill", ClampMin = "0.0"))
       float MaxChargeSeconds = 1.5f;

       // Skill의 최대 콤보 횟수
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bHasCombo", ClampMin = "1"))
       int32 MaxComboCount = 1;

       // Skill의 콤보 유지 시간
       UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bHasCombo", ClampMin = "0.0"))
       float ComboResetTime = 1.0f;
    #pragma endregion

	// Skill의 런타임 데이터 (쿨다운, 콤보 등)
	#pragma region Skill Runtime Data
	    // Skill의 Cooldown 남은 시간
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    float CooldownRemaining = 0.0f;

	    // Skill의 현재 Combo 횟수
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    int32 CurrentComboCount = 0;

	    // Skill의 Charging 진행 시간
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    float ChargingTime = 0.0f;

	    // [추가] 콤보 유지를 위한 타이머 (내부 로직용)
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    float CurrentComboTimer = 0.0f;

	    // [추가] 현재 스킬의 상태
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    ESkillState CurrentState = ESkillState::Ready;
	#pragma endregion

	// Skill의 애니메이션 (Flipbook)
	#pragma region Flipbooks
	    /**
	     * Skill을 Cast하기 위한 Charge를 진행하기 위해 Idle에서 전이할 때 재생할 Flipbook\n
	     * 없을 경우 바로 Charging State로 들어감
	     */
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
	    UPaperFlipbook* PreChargingFlipbook = nullptr;

	    /**
	     * Skill을 Cast하기 위한 Charge를 진행하는 과정 중에 재생할 Flipbook\n
	     * 없을 경우 바로 시전\n
	     * Charging Skill의 경우 이 Flipbook의 재생 시간이 최대 Charging 시간으로 간주됨
	     */
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
	    UPaperFlipbook* ChargingFlipbook = nullptr;
	    
	    /**
	     * 최대 Charging 시간 이후 Charging 유지시 Loop할 Charging Flipbook\n
	     * 없을 경우 최대 Charging 시간 이후 대기 없이 바로 시전\n
	     * 만약 존재할 경우 플레이어가 시전 명령을 내릴 때까지(대부분 해당 Skill 키를 떼면서 발생) 이 Flipbook이 반복 재생됨
	     */
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
	    UPaperFlipbook* PostChargingFlipbook = nullptr;

	    /**
	     * Casting 할 때 재생할 Flipbook (공격 판정은 Casting 시점에 발생)\n
	     * 이 Animation이 종료될 때 다른 Skill을 사용할 수 있음\n
	     * [참고] 콤보 스킬일 경우 0번 인덱스부터 순서대로 사용됨 (없으면 기본값 사용)
	     */
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
	    UPaperFlipbook* CastingFlipbook = nullptr;

	    // [추가] 콤보 애니메이션 지원을 위한 배열 (옵션)
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook", meta=(EditCondition="bHasCombo"))
	    TArray<UPaperFlipbook*> ComboFlipbooks;

	    /**
	     * Casting 이후 Idle로 복귀하기 전까지 재생할 Flipbook\n
	     * 이 Skill의 재생 중에 다른 Skill을 사용할 수 있음
	     */
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
	    UPaperFlipbook* PostCastingFlipbook = nullptr;
	    
	#pragma endregion
#pragma endregion

#pragma region Methods
public:
    // [입력] 키 누름
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void TryStartSkill();

    // [입력] 키 뗌 (차징용)
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void TryStopSkill();

    // 강제 취소
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void CancelSkill();

    // 델리게이트 (외부 바인딩용)
    UPROPERTY(BlueprintAssignable, Category = "Skill")
    FOnSkillExecuted OnSkillExecuted;

protected:
	// 남은 쿨타임 계산 (매 Tick마다 업데이트)
	float CalculateCooldownRemains(float DeltaTime);

	// 남은 콤보 유지 시간 계산 (매 Tick마다 업데이트)
	float CalculateComboResetRemains(float DeltaTime);

	// 차징 진행도 계산 (매 Tick마다 업데이트)
	float CalculateChargingProgress(float DeltaTime);

	void USkillBase::ApplyCooldown();
	
    // 상태 변경 및 애니메이션 요청
    void SetState(ESkillState NewState);
    
    // 데미지 계산 (고정/난수/차징 반영)
    virtual float CalculateDamage() const;

    // 실제 발동 로직
    virtual void ExecuteSkillLogic();
    
    // 콤보 리셋
    void ResetCombo();

    // 오너에게 애니메이션 재생 요청
    void RequestPlayAnimation(UPaperFlipbook* Flipbook, bool bLoop);
#pragma endregion
};