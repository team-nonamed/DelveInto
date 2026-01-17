#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PaperFlipbookComponent.h"
#include "DelveEnemy.generated.h"

UCLASS()
class DELVEINTO_API ADelveEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ADelveEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UPaperFlipbookComponent> EnemyFlipbook;

	// --- 스탯 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float WalkSpeed = 300.0f;

	bool bIsDead = false;

	// 피격 시 색상 변경 타이머
	FTimerHandle HitFlashTimer;

	// 색상을 원래대로 돌리는 함수
	void ResetSpriteColor();

public:
	// 데미지 받는 함수 (언리얼 기본 함수 오버라이드)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual bool IsDead() const { return bIsDead; }

private:
	// 2D 스프라이트가 항상 카메라를 보게 하는 함수 (빌보드)
	void UpdateBillboardRotation();
    
	// 플레이어와의 각도를 계산해 8방향 중 어디를 보여줄지 결정하는 함수
	void DetermineSpriteDirection();
};