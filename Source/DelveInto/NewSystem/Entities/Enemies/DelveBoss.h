#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Entities/DelveEnemy.h"
#include "NewSystem/Widgets/BossHealthWidget.h"
#include "DelveBoss.generated.h"

// 보스의 공격 패턴 (Walk는 삭제됨!)
UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
    None,
    Swipe,  
    Slam,   
    Charge, 
    Roar,       // 돌진 전 포효
    FirstRoar   // [신규] 최초 조우 포효
};

UCLASS()
class DELVEINTO_API ADelveBoss : public ADelveEnemy
{
    GENERATED_BODY()

public:
    ADelveBoss();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // [신규] 스폰 시 최초 포효를 위한 전용 함수들
    void PerformSpawnRoar();
    void ExecuteSpawnRoarAction();
    void FinishSpawnRoar();

    FTimerHandle SpawnRoarTimer;
    
    // --- 부모 클래스의 공격 흐름 오버라이드 ---
    virtual void StartAttackSequence(AActor* Target) override;
    virtual void ExecuteAttack() override;
    virtual void FinishAttack() override;

    // ==========================================
    // [보스 전용] 패턴 및 애니메이션 (플립북)
    // ==========================================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss | Combat")
    EBossAttackPattern CurrentPattern;

    void ChooseNextPattern();

    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Swipe")
    TObjectPtr<class UPaperFlipbook> SwipePrepFlipbook;
    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Swipe")
    TObjectPtr<class UPaperFlipbook> SwipeActionFlipbook;

    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Slam")
    TObjectPtr<class UPaperFlipbook> SlamPrepFlipbook;
    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Slam")
    TObjectPtr<class UPaperFlipbook> SlamActionFlipbook;

    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Charge")
    TObjectPtr<class UPaperFlipbook> ChargePrepFlipbook;
    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Charge")
    TObjectPtr<class UPaperFlipbook> ChargeActionFlipbook;

    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Roar")
    TObjectPtr<class UPaperFlipbook> RoarPrepFlipbook;
    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Roar")
    TObjectPtr<class UPaperFlipbook> RoarActionFlipbook;

    virtual void PerformMeleeDamageCheck() override;

    // [신규] 이번 공격 패턴에서 이미 데미지를 준 대상들을 기억하는 명단
    UPROPERTY()
    TArray<AActor*> HitActorsThisAttack;
    
    // ==========================================
    // [보스 전용] 패턴별 상세 로직
    // ==========================================
    void PerformSwipe();
    void PerformSlam();
    void PerformCharge();
    void PerformRoar();

    void LandSlam();
    FTimerHandle SlamLandTimer;

    bool bIsChargingForward = false;
    FVector ChargeDirection;

    // ==========================================
    // [보스 전용] 데스 시퀀스 & UI
    // ==========================================
    UPROPERTY(EditAnywhere, Category = "Boss | Sequence")
    TSubclassOf<class UUserWidget> DemoEndWidgetClass;

    void StartDeathSequence();
    void TriggerFadeOut();
    void ShowThankYouScreen();

    FTimerHandle DeathSequenceTimer;
    FTimerHandle FadeTimer;

    UPROPERTY()
    class ACameraActor* CinematicCamera;

    // ==========================================
    // [보스 전용] 패턴별 사운드
    // ==========================================
    
    // 1. 휘두르기 (Swipe)
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Swipe")
    class USoundBase* SwipePrepSound;
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Swipe")
    class USoundBase* SwipeActionSound;

    // 2. 내려찍기 (Slam)
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Slam")
    class USoundBase* SlamPrepSound;
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Slam")
    class USoundBase* SlamActionSound;

    // 3. 돌진 (Charge)
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Charge")
    class USoundBase* ChargePrepSound;
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Charge")
    class USoundBase* ChargeActionSound;

    // 4. 포효 (Roar)
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Roar")
    class USoundBase* RoarPrepSound;
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Roar")
    class USoundBase* RoarActionSound;

    // [신규] 첫 조우 포효 사운드
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Roar")
    class USoundBase* FirstRoarSound; 
    
    // (기존의 RoarActionSound는 대시 전 포효 사운드로 그대로 사용합니다)

    // [신규] 상태 기억용 변수
    bool bHasDoneFirstRoar = false;
    EBossAttackPattern ForcedNextPattern = EBossAttackPattern::None;

    // ==========================================
    // [보스 전용] 돌진(Charge) 세부 설정
    // ==========================================
    
    // 돌진을 유지할 시간 (초). 애니메이션 길이보다 길게 설정하면 멀리 갑니다!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat | Charge")
    float CustomChargeDuration = 1.5f; 

    // 돌진 시 속도 뻥튀기 배율 (기존엔 3.0 고정이었음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat | Charge")
    float ChargeSpeedMultiplier = 4.0f;

    // ==========================================
    // [보스 전용] 내려찍기(Slam) 세부 설정
    // ==========================================

    // [신규] 바닥에 쾅! 착지했을 때의 모션
    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Slam")
    TObjectPtr<class UPaperFlipbook> SlamLandFlipbook;

    // [신규] 내려찍기 피해 반경 (블루프린트에서 조절 가능하게 노출)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat | Slam")
    float SlamHitRadius = 600.0f; // 기존 500에서 600으로 기본값 증가!

    // [신규] 바닥에 쾅! 찍을 때 나는 폭발음
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Slam")
    class USoundBase* SlamLandSound;

    // ==========================================
    // [신규] 보스 UI 설정
    // ==========================================
    
    // 블루프린트에서 할당할 보스 체력바 위젯 클래스
    UPROPERTY(EditAnywhere, Category = "Boss | UI")
    TSubclassOf<UBossHealthWidget> BossHealthWidgetClass;

    // 화면에 띄운 위젯을 기억해둘 리모컨
    UPROPERTY()
    UBossHealthWidget* BossHealthWidgetInstance;

    // 보스 이름 (블루프린트에서 "심연의 감시자" 등으로 설정 가능)
    UPROPERTY(EditAnywhere, Category = "Boss | UI")
    FText BossName = FText::FromString(TEXT("???"));

    // ==========================================
    // [보스 전용] 발소리 (Footstep) 세팅
    // ==========================================

    // 보스 발소리 사운드
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Walk")
    class USoundBase* FootstepSound;

    // 발소리가 나야 하는 플립북 프레임 번호들 (예: 2, 5번 프레임에서 발이 닿는다면 [2, 5] 입력)
    UPROPERTY(EditAnywhere, Category = "Boss | Audio | Walk")
    TArray<int32> FootstepFrames;

    // 중복 재생을 막기 위해 마지막으로 소리를 낸 프레임을 기억하는 변수
    int32 LastFootstepFrame = -1;

    // ==========================================
    // [보스 전용] 포효(Roar) 특수 효과
    // ==========================================
    
    // 1. 화면이 지진 난 것처럼 흔들리는 카메라 쉐이크
    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Roar Effect")
    TSubclassOf<class UCameraShakeBase> RoarCameraShakeClass;

    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Roar Effect")
    TSubclassOf<class UCameraShakeBase> FirstRoarCameraShakeClass;

    // 2. 화면 테두리가 붉어지거나 왜곡되는 특수 효과 위젯
    UPROPERTY(EditAnywhere, Category = "Boss | Visuals | Roar Effect")
    TSubclassOf<class UUserWidget> RoarScreenEffectWidgetClass;

    // 포효 효과를 터뜨리는 헬퍼 함수
    void PlayRoarScreenEffect();

    void PlayFirstRoarScreenEffect();
};