#pragma once

#include "CoreMinimal.h"
#include "DelveCharacter.h"
#include "WeaponBase.h"
#include "Longsword.generated.h"

UCLASS()
class DELVEINTO_API ALongsword : public AWeaponBase
{
	GENERATED_BODY()
    
public:
	virtual void TryPrimaryAttack() override;
	virtual void TrySecondaryAttack(bool bIsHolding) override;
	virtual void TrySkillQ() override;
	virtual void TrySkillE() override;

	

protected:
	// 평타 3단 콤보 애니메이션 (에디터에서 순서대로 등록: 좌 -> 우 -> 찍기)
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Animations")
	TArray<TObjectPtr<UPaperFlipbook>> PrimaryComboAnims;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Animations")
	TObjectPtr<UPaperFlipbook> ChargePrepFlipbook;

#pragma region Alter Attack
	// --- 차징 3단계 애니메이션 ---
	UPROPERTY(EditAnywhere, Category = "Combat | Charge Visual")
	TObjectPtr<UPaperFlipbook> AlterPrepareFlipbook; // 1단계: 준비 (Loop X)

	UPROPERTY(EditAnywhere, Category = "Combat | Charge Visual")
	TObjectPtr<UPaperFlipbook> AlterChargeFlipbook;  // 2단계: 기 모으기 (Loop O)

	UPROPERTY(EditAnywhere, Category = "Combat | Charge Visual")
	TObjectPtr<UPaperFlipbook> AlterCastFlipbook;    // 3단계: 발사 (Loop X)

	// --- 내부 로직용 ---
	FTimerHandle PrepareTimerHandle; // 준비 -> 차징 전환용 타이머

	// 타이머가 끝나면 호출될 함수
	void StartChargeLoop();

#pragma endregion

private:
	// 콤보 시스템
	int32 CurrentComboIndex = 0;
	FTimerHandle ComboResetTimer;
	void ResetCombo();
	
	// --- 차징 및 투사체 설정 ---
	UPROPERTY(EditAnywhere, Category = "Combat | Special")
	TSubclassOf<class ADelveProjectile> SwordWaveClass;

	UPROPERTY(EditAnywhere, Category = "Combat | Special")
	float MaxChargeTime = 1.5f;

	// --- 내부 상태 ---
	bool bIsCharging = false;
	float ChargeStartTime = 0.0f;

	// // 실제 발사 로직
	// void FireSwordWave(float ChargeRatio);

	// 최대 차징 시 데미지 (투사체에 전달용)
	UPROPERTY(EditAnywhere, Category = "Combat | Special")
	float MaxChargeDamage = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Combat | Special")
	float MinChargeDamage = 10.0f;

	void FireSwordWave(float ChargeRatio);
};