// DelveProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h" // [추가] 포인트 라이트
#include "NiagaraComponent.h"               // [추가] 나이아가라 컴포넌트
#include "NiagaraSystem.h"                  // [추가] 나이아가라 시스템 에셋
#include "DelveProjectile.generated.h"

UCLASS()
class DELVEINTO_API ADelveProjectile : public AActor
{
    GENERATED_BODY()
    
public:    
    ADelveProjectile();

protected:
    virtual void BeginPlay() override;

public:
    // --- 컴포넌트 ---
    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    USphereComponent* CollisionComp;

    // [변경] 날아갈 때 보이는 메인 이펙트 (Trail 등)
    UPROPERTY(VisibleAnywhere, Category = "Visual")
    UNiagaraComponent* ProjectileEffect; 

    // [추가] 탄환 자체의 발광 효과 (BP_Projectile_6에 있음)
    UPROPERTY(VisibleAnywhere, Category = "Visual")
    UPointLightComponent* ProjectileLight;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;

    // --- 이펙트 에셋 설정 (블루프린트에서 할당) ---
    
    // [추가] 발사 순간 터지는 섬광 (Flash Effect)
    UPROPERTY(EditAnywhere, Category = "Visual | VFX")
    UNiagaraSystem* MuzzleFlashEffect;

    // [추가] 충돌 시 터지는 이펙트 (Hit Effect)
    UPROPERTY(EditAnywhere, Category = "Visual | VFX")
    UNiagaraSystem* HitEffect;

    // --- 기본 스탯 ---
    UPROPERTY(EditAnywhere, Category = "Stats")
    float BaseDamage = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float MoveSpeed = 1500.0f; // 마켓플레이스 에셋은 보통 속도가 빠름

    UPROPERTY(EditAnywhere, Category = "Stats")
    float LifeTime = 3.0f;

    // --- 차징 스탯 ---
    UPROPERTY(EditAnywhere, Category = "Stats | Player Charge")
    float MinDamage = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Stats | Player Charge")
    float MaxDamage = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Stats | Player Charge")
    float MinSpeed = 1500.0f;

    UPROPERTY(EditAnywhere, Category = "Stats | Player Charge")
    float MaxSpeed = 3000.0f;

    // --- 기능 ---
    void InitializeChargeStats(float ChargeRatio);

private:
    float CurrentDamage;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};