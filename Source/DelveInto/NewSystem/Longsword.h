#pragma once

#include "CoreMinimal.h"
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

private:
	// 콤보 시스템
	int32 CurrentComboIndex = 0;
	FTimerHandle ComboResetTimer;
	void ResetCombo();

	// 차징 시스템
	float ChargeStartTime = 0.0f;
	const float MaxChargeTime = 2.0f;
};