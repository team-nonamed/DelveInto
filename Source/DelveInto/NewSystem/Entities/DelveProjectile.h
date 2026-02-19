#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DelveProjectile.generated.h"

class USphereComponent;
class UPointLightComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;

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

    UPROPERTY(VisibleAnywhere, Category = "Visual")
    UNiagaraComponent* ProjectileEffect; 

    UPROPERTY(VisibleAnywhere, Category = "Visual")
    UPointLightComponent* ProjectileLight;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;

    // --- 시각 효과 ---
    UPROPERTY(EditAnywhere, Category = "Visual | VFX")
    UNiagaraSystem* MuzzleFlashEffect;

    UPROPERTY(EditAnywhere, Category = "Visual | VFX")
    UNiagaraSystem* HitEffect;

    // --- 기본 스탯 ---
    UPROPERTY(EditAnywhere, Category = "Stats")
    float BaseDamage = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float MoveSpeed = 1500.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float LifeTime = 3.0f;

    // --- 차징 스탯 ---
    UPROPERTY(EditAnywhere, Category = "Stats | Charge")
    float MinDamage = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Stats | Charge")
    float MaxDamage = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Stats | Charge")
    float MinSpeed = 1500.0f;

    UPROPERTY(EditAnywhere, Category = "Stats | Charge")
    float MaxSpeed = 3000.0f;

    // --- 기능 ---
    void InitializeChargeStats(float ChargeRatio);

private:
    float CurrentDamage;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};