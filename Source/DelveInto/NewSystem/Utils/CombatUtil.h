#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatUtil.generated.h"

UCLASS()
class DELVEINTO_API UCombatUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 구형 범위 데미지 판정
	 * @param WorldContextObject 월드 컨텍스트 (BP에서 자동 할당)
	 * @param Instigator 스킬을 시전한 주체 (캐릭터 등)
	 * @param Damage 데미지량
	 * @param Radius 구의 반지름
	 * @param OriginOffset 시전자 기준 오프셋 (앞, 오른쪽, 위쪽)
	 * @param DamageCauser 실제 데미지를 입히는 오브젝트 (무기, 투사체 등. 생략 시 Instigator로 대체)
	 * @return 적중 여부
	 */
	UFUNCTION(BlueprintCallable, Category="Combat|Damage", meta=(WorldContext="WorldContextObject"))
	static bool ApplyDamageSphere(const UObject* WorldContextObject, AActor* Instigator, float Damage, float Radius, FVector OriginOffset = FVector::ZeroVector, AActor* DamageCauser = nullptr);

	/**
	 * 2D 부채꼴 (원기둥 기반) 범위 데미지 판정
	 * @param HalfAngle 시선 기준 허용 각도의 절반 (예: 45도면 총 90도 범위)
	 * @param HalfHeight 판정 높이의 절반
	 */
	UFUNCTION(BlueprintCallable, Category="Combat|Damage", meta=(WorldContext="WorldContextObject"))
	static bool ApplyDamageFanShape(const UObject* WorldContextObject, AActor* Instigator, float Damage, float Radius, float HalfAngle, float HalfHeight, AActor* DamageCauser = nullptr);

	/**
	 * 3D 구형 부채꼴 판정 (카메라 시선 기준 정밀 판정)
	 */
	UFUNCTION(BlueprintCallable, Category="Combat|Damage", meta=(WorldContext="WorldContextObject"))
	static bool ApplyDamageSphericalCone(const UObject* WorldContextObject, AActor* Instigator, float Damage, float Radius, float HalfAngle, AActor* DamageCauser = nullptr);

	/** 두 액터가 적대 관계인지 확인합니다. (플레이어 vs 적) */
	UFUNCTION(BlueprintPure, Category="Combat|Faction")
	static bool IsEnemy(AActor* ActorA, AActor* ActorB);

	/** 두 액터가 아군 관계인지 확인합니다. (플레이어 vs 플레이어, 적 vs 적) */
	UFUNCTION(BlueprintPure, Category="Combat|Faction")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);

	/** 단일 타겟에게 데미지를 가합니다. (적대 관계일 때만 적용) */
	UFUNCTION(BlueprintCallable, Category="Combat|Damage", meta=(WorldContext="WorldContextObject"))
	static bool ApplyDamageIfEnemy(const UObject* WorldContextObject, AActor* Instigator, AActor* Target, float Damage, AActor* DamageCauser = nullptr);
};