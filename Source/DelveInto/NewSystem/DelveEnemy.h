#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "DelveEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeathSignature, class ADelveEnemy*, DeadEnemy);

UCLASS()
class DELVEINTO_API ADelveEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    ADelveEnemy();

    // 캡슐 크기에 맞춰 스프라이트 자동 조절
    UPROPERTY(EditAnywhere, Category = "Visual")
    bool bAutoResizeToCapsule = true;

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    class UWidgetComponent* HealthBarWidget;

public:
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // --- 공통 상태 변수 ---
    UPROPERTY(EditAnywhere, Category = "Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsDead = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsAttacking = false;

    // --- 공격 설정 ---
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackCooldown = 1.5f;

    // --- 비주얼 (플립북) ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
    TObjectPtr<UPaperFlipbookComponent> EnemyFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> IdleFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> ForwardFlipbook;

    // [변경] 공격 "준비(차징)" 모션 (Jumper는 움츠리기, 기본은 기모으기)
    UPROPERTY(EditAnywhere, Category = "Visual | Attack")
    TObjectPtr<UPaperFlipbook> AttackPrepFlipbook;

    // [변경] 공격 "실행" 모션 (Jumper는 타격용, 기본은 휘두르기)
    UPROPERTY(EditAnywhere, Category = "Visual | Attack")
    TObjectPtr<UPaperFlipbook> AttackFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> DeathFlipbook;

    // --- [핵심] 공격 시스템 함수 ---
    
    // 외부(AI Controller)에서 호출: 공격 시퀀스 시작 (차징 시작)
    virtual void StartAttackSequence(AActor* Target);

    // 공격 후딜레이가 끝나고 호출 (쿨타임 시작, Idle 복귀)
    virtual void FinishAttack();

    // [추가] 플레이어를 감지하고 추격을 시작할 거리 (단위: cm)
    UPROPERTY(EditAnywhere, Category = "AI")
    float DetectRange = 700.0f;

    UPROPERTY(EditAnywhere, Category = "Damage")
    float Damage = 10.0f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    class UBoxComponent* BodyCollision;
    
    // 차징 끝난 후 실행되는 로직 (자식이 오버라이드 가능)
    virtual void ExecuteAttack(); 
    
    // 실제 근접 데미지 판정 (Sphere Trace)
    void PerformMeleeDamageCheck();

    // --- 내부 변수 ---
    UPROPERTY()
    AActor* CachedTarget; // 공격 대상 저장

    bool bCanAttack = true;
    
    FTimerHandle AttackPrepTimer;     // 차징 시간
    FTimerHandle AttackActionTimer;   // 공격 모션 시간
    FTimerHandle AttackCooldownTimer; // 쿨타임
    FTimerHandle AnimResetTimer;      
    FTimerHandle HitFlashTimer;
    FTimerHandle DistanceDebugTimer;
    FTimerHandle DeathTimer;

    // 유틸리티
    void ResetCooldown();
    virtual void ReturnToIdle();
    void ResetSpriteColor();
    void PrintDistanceToPlayer();
    void UpdateAnimation();
    void DestroySelf();               // 액터 삭제 함수 (추가)

    // [추가] 항상 플레이어를 바라볼 것인가?
    UPROPERTY(EditAnywhere, Category = "Combat")
    bool bAlwaysFacePlayer = true;

    // [추가] 회전 속도 (높을수록 빠름)
    UPROPERTY(EditAnywhere, Category = "Combat")
    float RotationSpeed = 10.0f; 

    // 회전 로직 함수
    void FaceToPlayer(float DeltaTime);

    // --- [추가] 오디오 설정 ---
    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* AttackPrepSound; // 차징(기모으기) 소리 (예: 크르릉..)

    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* AttackSound; // 공격(휘두르기) 소리 (예: 슉!)

    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* HitSound; // 맞았을 때 소리

    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* DeathSound; // 죽었을 때 소리
    
    // [추가] 추격을 포기하고 돌아갈 거리 (감지 거리보다 약간 크게 잡으면 자연스러움)
    UPROPERTY(EditAnywhere, Category = "AI")
    float GiveUpRange = 800.0f;

    // 현재 플레이어를 추격 중인지 확인하는 함수 (AIController에서 사용)
    bool IsPlayerInDetectRange() const;

public:
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEnemyDeathSignature OnEnemyDeath;
};