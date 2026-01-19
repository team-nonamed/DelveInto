#pragma once

#include "CoreMinimal.h"
#include "DelveEnemy.h" // 부모 헤더 필수
#include "DelveEnemy_Jumper.generated.h"

UCLASS()
class DELVEINTO_API ADelveEnemy_Jumper : public ADelveEnemy
{
	GENERATED_BODY()

public:
	// AI가 호출할 점프 공격
	void JumpAttack(AActor* Target);

	UPROPERTY(EditAnywhere, Category = "Combat")
	float JumpAttackRange = 500.0f;

protected:
	bool bCanJumpAttack = true;

	// [추가] 점프 준비 동작 (움츠리기)
	UPROPERTY(EditAnywhere, Category = "Visual")
	class UPaperFlipbook* JumpStartFlipbook; 

	// [추가] 점프 타이머 및 타겟 저장용
	FTimerHandle JumpDelayTimer;
    
	// 타이머가 끝나면 호출될 실제 점프 함수
	void ExecuteJump(); 

	// 점프 대상(플레이어)을 잠깐 저장해둘 변수
	UPROPERTY()
	AActor* CachedJumpTarget;

	

	// [추가] 점프 공격용 애니메이션들
	UPROPERTY(EditAnywhere, Category = "Visual")
	class UPaperFlipbook* JumpFlipbook;     // 날아갈 때 모션
	
	bool bHasHitTargetInAir = false; // 공중에서 이미 때렸는지 체크용

	// [필수] 충돌 감지 (언리얼 기본 함수 오버라이드)
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	virtual void Landed(const FHitResult& Hit) override;

	void ResetJumpCooldown();
	void ApplyLandingDamage();
};