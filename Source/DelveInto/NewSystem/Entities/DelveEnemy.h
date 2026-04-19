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

    // ==========================================
    // [신규] 보상 설정
    // ==========================================
    UPROPERTY(EditAnywhere, Category = "Reward")
    int32 DropGoldAmount = 8;

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

    // 공격 "준비(차징)" 모션
    UPROPERTY(EditAnywhere, Category = "Visual | Attack")
    TObjectPtr<UPaperFlipbook> AttackPrepFlipbook;

    // 공격 "실행" 모션
    UPROPERTY(EditAnywhere, Category = "Visual | Attack")
    TObjectPtr<UPaperFlipbook> AttackFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> DeathFlipbook;

    // --- [핵심] 공격 시스템 함수 ---
    virtual void StartAttackSequence(AActor* Target);
    virtual void FinishAttack();

    UPROPERTY(EditAnywhere, Category = "AI")
    float DetectRange = 700.0f;

    UPROPERTY(EditAnywhere, Category = "Damage")
    float Damage = 10.0f;
    
public:
    // 다른 클래스나 블루프린트에서 호출할 수 있도록 UFUNCTION을 붙여줍니다.
    UFUNCTION(BlueprintCallable, Category = "Enemy|Visual")
    FVector GetFlipbookScale() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    class UBoxComponent* BodyCollision;
    
    virtual void ExecuteAttack(); 
    virtual void PerformMeleeDamageCheck();

    UPROPERTY()
    AActor* CachedTarget; 

    bool bCanAttack = true;
    
    FTimerHandle AttackPrepTimer;     
    FTimerHandle AttackActionTimer;   
    FTimerHandle AttackCooldownTimer; 
    FTimerHandle AnimResetTimer;      
    FTimerHandle HitFlashTimer;
    FTimerHandle DistanceDebugTimer;
    FTimerHandle DeathTimer;

    void ResetCooldown();
    virtual void ReturnToIdle();
    void ResetSpriteColor();
    void PrintDistanceToPlayer();
    virtual void UpdateAnimation();
    void DestroySelf();               
    
    UPROPERTY(EditAnywhere, Category = "Combat")
    bool bAlwaysFacePlayer = true;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float RotationSpeed = 10.0f; 

    void FaceToPlayer(float DeltaTime);

    // --- 오디오 설정 ---
    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* AttackPrepSound; 

    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* AttackSound; 

    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* HitSound; 

    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* DeathSound; 
    
    UPROPERTY(EditAnywhere, Category = "AI")
    float GiveUpRange = 800.0f;

    bool IsPlayerInDetectRange() const;

public:
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEnemyDeathSignature OnEnemyDeath;
};