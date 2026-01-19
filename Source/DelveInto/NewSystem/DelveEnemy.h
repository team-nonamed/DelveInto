#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "DelveEnemy.generated.h"

UCLASS()
class DELVEINTO_API ADelveEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    ADelveEnemy();

    // [추가] 캡슐 크기에 맞춰 자동으로 스프라이트 크기를 조절할지 여부
    UPROPERTY(EditAnywhere, Category = "Visual")
    bool bAutoResizeToCapsule = true;

protected:
    virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override;

public:
    virtual void Tick(float DeltaTime) override;
    
    // 데미지 받는 함수 (빨간색 피격 효과 포함)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // --- 공통 상태 변수 ---
    UPROPERTY(EditAnywhere, Category = "Combat")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsDead = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsAttacking = false;

    // --- 공격 설정 ---
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackCooldown = 1.5f;

    // 공격 딜레이 타이머
    FTimerHandle AttackCooldownTimer;

    // --- 비주얼 (플립북) ---
    // 블루프린트에서 컴포넌트를 할당하거나, BeginPlay에서 자동으로 찾습니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
    TObjectPtr<UPaperFlipbookComponent> EnemyFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> IdleFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> ForwardFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> AttackFlipbook;

    UPROPERTY(EditAnywhere, Category = "Visual")
    TObjectPtr<UPaperFlipbook> DeathFlipbook;

    // --- 기능 함수 ---
    // AI가 호출하는 기본 공격 함수 (가상 함수)
    virtual void Attack();

protected:
    bool bCanAttack = true;
    FTimerHandle AttackTimerHandle; // 쿨타임용
    FTimerHandle AnimResetTimer;    // 애니메이션 복귀용
    FTimerHandle HitFlashTimer;     // 피격 효과용
    FTimerHandle DistanceDebugTimer;// 거리 로그용

    // 내부 로직
    void ResetCooldown();
    virtual void ReturnToIdle();
    void ResetSpriteColor();
    void PrintDistanceToPlayer();

    //매 프레임 애니메이션 상태를 갱신하는 함수
    void UpdateAnimation();
    
    // 실제 데미지 판정 (Attack 함수 안에서 호출)
    void PerformAttackCheck();
};