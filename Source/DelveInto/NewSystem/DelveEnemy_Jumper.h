#pragma once

#include "CoreMinimal.h"
#include "DelveEnemy.h"
#include "DelveEnemy_Jumper.generated.h"

UCLASS()
class DELVEINTO_API ADelveEnemy_Jumper : public ADelveEnemy
{
	GENERATED_BODY()

public:
	// 점프 공격 사거리 (AI 판단용)
	UPROPERTY(EditAnywhere, Category = "Combat")
	float JumpAttackRange = 500.0f;

protected:
	// [중요] 부모의 근접 공격을 '점프'로 바꿔치기
	virtual void ExecuteAttack() override;

	// 공중 전용 애니메이션 (준비 동작은 부모의 AttackPrepFlipbook 사용)
	UPROPERTY(EditAnywhere, Category = "Visual | Jump")
	class UPaperFlipbook* JumpAirFlipbook; 
    
	bool bHasHitTargetInAir = false; 

	// 충돌 및 착지 감지
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
    
	virtual void Landed(const FHitResult& Hit) override;

	void ApplyLandingDamage();
};