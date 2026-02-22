#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Entities/DelveEnemy.h"
#include "NewSystem/Interfaces/Interactable.h"
#include "Mimic.generated.h"

class UPaperFlipbook;
class USoundBase;

/**
 * ADelveEnemy를 상속받아 기존 몬스터 시스템을 활용하며,
 * IInteractableInterface를 통해 위장 해제 이벤트를 처리합니다.
 */
UCLASS()
class DELVEINTO_API AMimic : public ADelveEnemy, public IInteractable
{
	GENERATED_BODY()

public:
	AMimic();

protected:
	virtual void BeginPlay() override;
    
	// 부모 클래스의 애니메이션 자동 갱신을 제어하기 위해 오버라이드
	virtual void UpdateAnimation() override;

	// 플레이어가 상호작용 대신 공격을 가했을 때의 처리
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// --- 시각 효과 데이터 (스프라이트 대신 플립북 사용) ---
	UPROPERTY(EditDefaultsOnly, Category = "Mimic|Visual")
	TObjectPtr<UPaperFlipbook> DisguiseFlipbook; // 위장 상태 (닫힌 상자)

	UPROPERTY(EditDefaultsOnly, Category = "Mimic|Visual")
	TObjectPtr<UPaperFlipbook> RevealFlipbook; // 깨어나는 모션

	// --- 청각 효과 데이터 ---
	UPROPERTY(EditAnywhere, Category = "Mimic|Audio")
	USoundBase* WakeUpSound;

	// --- 전투 수치 ---
	UPROPERTY(EditAnywhere, Category = "Mimic|Combat")
	float SurpriseDamage = 30.0f;

	// 상태 추적
	bool bIsDisguised = true;
	bool bIsRevealing = false; // 깨어나는 애니메이션 재생 중인지 확인

	// 깨어나는 애니메이션 종료 후 호출될 타이머 및 함수
	FTimerHandle RevealTimer;
	void FinishReveal();

public:
	// IInteractableInterface 구현부
	virtual void Interact_Implementation(AActor* Interactor) override;

	// 내부적으로 위장을 해제하는 공통 로직
	void WakeUp(AActor* TargetActor);
};