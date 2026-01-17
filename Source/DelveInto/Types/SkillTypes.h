#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "SkillTypes.generated.h"

UENUM(BlueprintType)
enum class ESkillSlot : uint8
{
	BaseAttack,
	AltAttack,
	SkillQ,
	SkillE,
	Ultimate,
};

UENUM(BlueprintType)
enum class ESkillCastType : uint8
{
	Tap,          // 눌렀을 때 바로 “발동 후보”
	HoldRelease,  // 누르고 있다가 떼면 발동(차징)
};

UENUM(BlueprintType)
enum class ESkillDelivery : uint8
{
	MeleeArea,    // 근접 범위(원/부채꼴)
	Projectile,   // 투사체
	SummonHost,   // 소환
};

UENUM(BlueprintType)
enum class ESkillShape : uint8
{
	Circle,
	Cone,
};

/**
 * Skill 실행에 필요한 “호스트/원점/전방” 등의 런타임 컨텍스트.
 * - Designator 같은 슬롯 의미론은 컨텍스트가 아니라 “호출자(AttackHandler)”가 알고 있어야 합니다.
 */
USTRUCT(BlueprintType)
struct FSkillContext
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY()
	FVector Origin = FVector::ZeroVector;

	UPROPERTY()
	FVector Forward = FVector::ForwardVector;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsToIgnore;

	UPROPERTY()
	bool bIgnoreZ = false;
};

USTRUCT(BlueprintType)
struct FDamageSpec
{
	GENERATED_BODY()

	// 단발 데미지
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float BaseDamage = 0.f;

	// 콤보 데미지(있으면 콤보 스킬로 해석)
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TArray<float> ComboDamages;

	// 차징 데미지( HoldRelease 에서 사용 )
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float MinChargedDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float MaxChargedDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage", meta=(ClampMin="0.0"))
	float MaxChargeTime = 0.f;

	// 기존 설계 호환: “Modifier 형태”도 남겨둠(선택)
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float Additive = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float MultiplierAdditive = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	bool bRequireLOS = false;
};

USTRUCT(BlueprintType)
struct FMeleeSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Melee")
	ESkillShape Shape = ESkillShape::Cone;

	UPROPERTY(EditDefaultsOnly, Category="Melee", meta=(ClampMin="0.0"))
	float Radius = 200.f;

	// Cone일 때만 의미
	UPROPERTY(EditDefaultsOnly, Category="Melee", meta=(ClampMin="0.0", ClampMax="180.0"))
	float HalfAngleDeg = 45.f;

	UPROPERTY(EditDefaultsOnly, Category="Melee", meta=(ClampMin="1"))
	int32 MaxTargets = 8;
};

USTRUCT(BlueprintType)
struct FProjectileSpec
{
	GENERATED_BODY()

	/**
	 * “프리팹” 느낌으로 지정하는 건 결국 “스폰할 클래스(BP 클래스 포함)”입니다.
	 * 인스턴스 포인터(AActor*)를 들고 있으면 스폰/복제/GC에서 꼬입니다.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<class AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	FName MuzzleSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category="Projectile", meta=(ClampMin="0.0"))
	float InitialSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="Projectile", meta=(ClampMin="0.0"))
	float LifeSeconds = 5.f;
};

USTRUCT(BlueprintType)
struct FSummonSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Summon")
	TSubclassOf<class AAbstractHost> SummonedHostClass;

	UPROPERTY(EditDefaultsOnly, Category="Summon")
	FVector SpawnOffset = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FSkillExecSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Exec")
	ESkillCastType CastType = ESkillCastType::Tap;

	UPROPERTY(EditDefaultsOnly, Category="Exec")
	ESkillDelivery Delivery = ESkillDelivery::MeleeArea;

	UPROPERTY(EditDefaultsOnly, Category="Exec", meta=(ClampMin="0.0"))
	float CooldownSeconds = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Exec")
	FDamageSpec Damage;

	UPROPERTY(EditDefaultsOnly, Category="Exec")
	FMeleeSpec Melee;

	UPROPERTY(EditDefaultsOnly, Category="Exec")
	FProjectileSpec Projectile;

	UPROPERTY(EditDefaultsOnly, Category="Exec")
	FSummonSpec Summon;

	// 콤보 입력 유효 시간(콤보 스킬에만 의미)
	UPROPERTY(EditDefaultsOnly, Category="Exec", meta=(ClampMin="0.0"))
	float ComboWindowSeconds = 0.6f;
};
