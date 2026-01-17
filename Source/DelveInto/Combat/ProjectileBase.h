#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hurts/HurtHandler.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

/**
 * 스킬 투사체의 최소 베이스.
 * - 충돌 시 IHurtHandler 대상에게 FHurtRequest를 전달.
 */
UCLASS()
class DELVEINTO_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();

	void InitPayload(AActor* InSender, float InDamage, float InLifeSeconds);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(Transient)
	TObjectPtr<AActor> Sender = nullptr;

	UPROPERTY(Transient)
	float Damage = 0.f;

	UFUNCTION() // 엔진 콜백용(내부용). “BP 노출” 목적이 아닙니다.
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
