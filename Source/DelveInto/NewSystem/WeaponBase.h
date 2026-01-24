#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbook.h"
// Component는 에디터 호환성을 위해 남겨두지만 실제로는 안 씁니다.
#include "DelveCharacter.h"
#include "PaperFlipbookComponent.h" 
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponSkillSlot : uint8
{
	Primary, Secondary, SkillQ, SkillE
};

UCLASS()
class DELVEINTO_API AWeaponBase : public AActor
{
	GENERATED_BODY()
    
public:	
	AWeaponBase();

protected:
	virtual void BeginPlay() override;

protected:
	// 에디터 호환용 (사용 안 함)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UPaperFlipbookComponent> WeaponFlipbook;

	// --- 애니메이션 데이터 ---
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Animations")
	TObjectPtr<UPaperFlipbook> IdleFlipbook;

	UPROPERTY(EditDefaultsOnly, Category = "Visual|Animations")
	TMap<EWeaponSkillSlot, TObjectPtr<UPaperFlipbook>> SkillAnimations;

	// --- 상태 ---
	bool bIsAttacking = false;
	TMap<EWeaponSkillSlot, double> CooldownMap;

public:
	virtual void TryPrimaryAttack() {}
	virtual void TrySecondaryAttack(bool bIsHolding) {}
	virtual void TrySkillQ() {}
	virtual void TrySkillE() {}
	
	UFUNCTION()
	void ReturnToIdle();

	UPROPERTY()
	TObjectPtr<ADelveCharacter> MyOwnerCharacter;

protected:
	// UI에 애니메이션 재생 요청
	float PlayAnimationBySlot(EWeaponSkillSlot Slot);
	float PlayAnimationDirectly(UPaperFlipbook* InFlipbook);
    
	

	bool IsOnCooldown(EWeaponSkillSlot Slot);
	void SetCooldown(EWeaponSkillSlot Slot, float Duration);
    
	UFUNCTION(BlueprintCallable, Category="Combat")
	void ApplyDamageSphere(float Damage, float Radius, FVector OriginOffset = FVector::ZeroVector);

	// [수정] 부채꼴 기둥 판정 함수
	// HalfHeight: 공격 높이의 절반 (예: 90.0f면 위아래 총 180cm 커버)
	UFUNCTION(BlueprintCallable, Category="Combat")
	void ApplyDamageFanShape(float Damage, float Radius, float HalfAngle, float HalfHeight);

	// [신규] 3D 구형 부채꼴 판정 (시선 기준)
	// Radius: 사거리 (구의 반지름)
	// HalfAngle: 시선 기준 허용 각도 (예: 45도면 총 90도 범위)
	UFUNCTION(BlueprintCallable, Category="Combat")
	void ApplyDamageSphericalCone(float Damage, float Radius, float HalfAngle);
};