#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PaperFlipbookComponent.h"
#include "DelvePlayerProjectile.generated.h"

UCLASS()
class DELVEINTO_API ADelvePlayerProjectile : public AActor
{
	GENERATED_BODY()
    
public:    
	ADelvePlayerProjectile();

protected:
	virtual void BeginPlay() override;

public:
	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	UPaperFlipbookComponent* ProjectileFlipbook;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// --- 차징 관련 설정 (에디터에서 조절) ---
    
	// 차징을 하나도 안 했을 때의 최소 데미지
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MinDamage = 10.0f;

	// 풀차징 시 최대 데미지
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxDamage = 50.0f;

	// 최소 사거리 (cm 단위)
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MinDistance = 200.0f;

	// 최대 사거리 (cm 단위)
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxDistance = 1000.0f;

	// 투사체 속도
	UPROPERTY(EditAnywhere, Category = "Stats")
	float TravelSpeed = 800.0f;

	// [선택] 타격 시 터지는 이펙트
	UPROPERTY(EditAnywhere, Category = "Visual")
	class UPaperFlipbook* HitEffectFlipbook;

	// --- 기능 함수 ---

	/**
	 * 투사체 발사 직후 호출하여 스탯을 결정하는 함수
	 * @param ChargeRatio : 0.0(최소) ~ 1.0(풀차징) 사이의 값
	 */
	void InitializeProjectile(float ChargeRatio);

private:
	// 결정된 실제 데미지 저장 변수
	float CurrentDamage;

	// 충돌 처리 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};