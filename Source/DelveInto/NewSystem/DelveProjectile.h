#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DelveProjectile.generated.h"

UCLASS()
class DELVEINTO_API ADelveProjectile : public AActor
{
	GENERATED_BODY()
    
public:    
	ADelveProjectile();

protected:
	// 2D 투사체 이미지를 위한 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	class UPaperFlipbookComponent* ProjectileFlipbook;

	// 투사체 이동을 담당하는 컴포넌트 (속도, 중력 등 제어)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	// 차징에 따른 최소/최대 능력치
	UPROPERTY(EditAnywhere, Category = "Charge Stats")
	float MinSpeed = 800.0f;
	UPROPERTY(EditAnywhere, Category = "Charge Stats")
	float MaxSpeed = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Charge Stats")
	float MinLifeSpan = 0.4f;
	UPROPERTY(EditAnywhere, Category = "Charge Stats")
	float MaxLifeSpan = 2.5f;

public:
	// 캐릭터가 발사할 때 호출할 초기화 함수
	void InitializeChargeStats(float ChargeRatio);
};