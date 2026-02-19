#pragma once

#include "CoreMinimal.h"
#include "ComboTerminationType.h"
#include "PaperFlipbook.h"
#include "SkillExecutionType.h"
#include "SkillState.h"
#include "Components/ActorComponent.h"
#include "SkillBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSkill, Log, All);

// [신규] 스킬 실행 결과를 알리기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillActivatedDelegate, USkillBase*, SkillInst);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStateTransitionDelegate, ESkillState, SkillState, UPaperFlipbook*, Flipbook, bool, IsLoopAnimation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillDeactivatedDelegate, USkillBase*, SkillInst);

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
		// 이 Skill을 실행하고 있는 도중 다른 Skill을 시전할 수 있는지 여부
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		bool bIsInterruptable = false;

		// true면 애니메이션이 끝나도 자동으로 PostCasting으로 넘어가지 않음
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		bool bEndCastingManually = false; 
	
		// [추가] 기능 토글 (EditCondition용 변수)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		bool bIsChargingSkill = false; // 차징 스킬인가?

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		bool bHasCombo = false;        // 콤보 스킬인가?

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		bool bIsAutoCasting = false;   // Post Charging 이후 바로 Cast를 할 것인지

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features", meta = (EditCondition = bHasCombo))
		EComboTerminationType ComboTerminationType = EComboTerminationType::Unavailable;	   // Max Combo에서 초기화 되나

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		bool bHasVariableDamage = false; // 난수 데미지인가?

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		bool bHasCooldown = true;      // 쿨타임이 있는가?

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Features")
		ESkillExecutionType ExecutionType;
       
       // -----------------------------------------------------------------

		// Skill의 기본 피해량
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(ClampMin = "0.0"))
		float BaseDamage = 10.0f;

		// Skill의 최대 피해량 (난수 데미지일 때 사용)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(ClampMin = "0.0", EditCondition = "bHasVariableDamage"))
		float MaxDamage = 20.0f;

		// Skill의 쿨타임 (초)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bHasCooldown", ClampMin = "0.0"))
		float MaxCooldownSeconds = 1.0f;

		// Skill의 최대 Charging 시간 (차징 스킬일 때 사용)
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bIsChargingSkill", ClampMin = "0.1"))
		float MaxChargingSeconds = 1.0f;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config")
		bool bCanFireEarly = false;

		// Skill의 최대 콤보 횟수
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bHasCombo", ClampMin = "1"))
		int32 MaxComboCount = 1;

		// Skill의 콤보 유지 시간
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config", meta=(EditCondition = "bHasCombo", ClampMin = "0.0"))
		float MaxComboDurationSeconds = 1.0f;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config")
		USoundBase* CastButHurtFailedSound = nullptr;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Config")
		USoundBase* CastButHurtSuccessSound = nullptr;
    #pragma endregion

	// Skill의 런타임 데이터 (쿨다운, 콤보 등)
	#pragma region Skill Runtime Data
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
		bool bIsActivated = false;
	
		// [추가] 현재 스킬의 상태
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
    	ESkillState CurrentState = ESkillState::Idle;
	
		UPROPERTY()
		bool bIsKeyPressed = false;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill | Runtime")
		float CastSpeed = 1.0;
	
	    // Skill의 Cooldown 남은 시간
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    float CooldownSeconds = 0.0f;

	    // Skill의 현재 Combo 횟수
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    int32 CurrentComboCount = 0;

	    // Skill의 Charging 진행 시간
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    float ChargingSeconds = 0.0f;

	    // [추가] 콤보 유지를 위한 타이머 (내부 로직용)
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill | Runtime")
	    float ComboDurationSeconds = 0.0f;

	    
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

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
		UPaperFlipbook* PreCastingFlipbook = nullptr;

		// [추가] 콤보 애니메이션 지원을 위한 배열 (옵션)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook", meta=(EditCondition="bHasCombo"))
		TArray<UPaperFlipbook*> PreCastingFlipbooks;
	
	    /**
	     * Casting 할 때 재생할 Flipbook (공격 판정은 Casting 시점에 발생)\n
	     * 이 Animation이 종료될 때 다른 Skill을 사용할 수 있음\n
	     * [참고] 콤보 스킬일 경우 0번 인덱스부터 순서대로 사용됨 (없으면 기본값 사용)
	     */
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
	    UPaperFlipbook* CastingFlipbook = nullptr;

	    // [추가] 콤보 애니메이션 지원을 위한 배열 (옵션)
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook", meta=(EditCondition="bHasCombo"))
	    TArray<UPaperFlipbook*> CastingFlipbooks;

	    /**
	     * Casting 이후 Idle로 복귀하기 전까지 재생할 Flipbook\n
	     * 이 Skill의 재생 중에 다른 Skill을 사용할 수 있음
	     */
	    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook")
	    UPaperFlipbook* PostCastingFlipbook = nullptr;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Flipbook", meta=(EditCondition="bHasCombo"))
		TArray<UPaperFlipbook*> PostCastingFlipbooks;
	    
	#pragma endregion

	#pragma region Timer Handles

		UPROPERTY()
		FTimerHandle PreChargingExpirationHandle;

		UPROPERTY()
		FTimerHandle ChargingExpirationHandle;

		UPROPERTY()
		FTimerHandle PostChargingExpirationHandle;

		UPROPERTY()
		FTimerHandle PreCastingExpirationHandle;

		UPROPERTY()
		FTimerHandle CastingExpirationHandle;

		UPROPERTY()
		FTimerHandle PostCastingExpirationHandle;
	
	#pragma endregion
#pragma endregion

#pragma region Methods
public:
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual bool HandleKeyPressed(float InCastSpeed = 1.0f);

    // [입력] 키 뗌 (차징용)
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual bool HandleKeyReleased();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual void DeactivateSkill();

protected:
	// 스킬의 상태를 바로 사용할 수 있도록 초기화
	virtual void ResetSkill();
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual void ActivateSkill();
	
	#pragma region Skill State Handling
		UFUNCTION(BlueprintCallable, Category = "Skill")
		virtual void HandlePreCharging();

		UFUNCTION(BlueprintCallable, Category = "Skill")
		virtual void HandleCharging();

		UFUNCTION(BlueprintCallable, Category = "Skill")
		virtual void HandlePostCharging();

		UFUNCTION(BlueprintCallable, Category = "Skill")
		virtual void HandlePreCasting();

		UFUNCTION(BlueprintCallable, Category = "Skill")
		virtual void HandleCasting();

		UFUNCTION(BlueprintCallable, Category = "Skill")
		virtual void HandlePostCasting(bool IsSuccess);
	#pragma endregion
	
	// 남은 쿨타임 계산 (매 Tick마다 업데이트)
	float CalculateCooldownSeconds(float DeltaTime);

	// 남은 콤보 유지 시간 계산 (매 Tick마다 업데이트)
	float CalculateComboDurationSeconds(float DeltaTime);

	// 차징 진행도 계산 (매 Tick마다 업데이트)
	float CalculateChargingSeconds(float DeltaTime);
    
    // 데미지 계산 (고정/난수/차징 반영)
    virtual float CalculateDamage() const;

    // 실제 발동 로직
	UFUNCTION(BlueprintNativeEvent, Category = "Skill")
	bool ExecuteSkillLogic();
	virtual bool ExecuteSkillLogic_Implementation();
    
    // 콤보 리셋
    void ResetCombo();

	UPaperFlipbook* GetFlipbookForCombo(const TArray<UPaperFlipbook*>& ComboFlipbooks, UPaperFlipbook* DefaultFlipbook) const;
#pragma endregion

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSkillActivatedDelegate OnSkillActivated;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTransitionDelegate OnPreCharging;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTransitionDelegate OnCharging;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTransitionDelegate OnPostCharging;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTransitionDelegate OnPreCasting;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTransitionDelegate OnCasting;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateTransitionDelegate OnPostCasting;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSkillDeactivatedDelegate OnSkillDeactivated;
};