#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatTargetingLibrary.generated.h"

/**
 * 타겟팅은 순수 함수에 가까우므로 라이브러리로 둡니다.
 * - Actor/Component에 상태를 붙이지 않으려는 목적.
 */
UCLASS()
class DELVEINTO_API UCombatTargetingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 원형 범위 검색(Overlap) + (선택) LOS 검사.
	 * 반환은 AActor* 리스트. 이후 호출부에서 IHurtHandler로 캐스팅/필터링합니다.
	 */
	static TArray<AActor*> FindActorsInCircle(
		UWorld* World,
		const FVector& Origin,
		float Radius,
		const FCollisionObjectQueryParams& ObjectQueryParams,
		ECollisionChannel TraceChannelForLOS,
		const TArray<AActor*>& ActorsToIgnore,
		bool bRequireLineOfSight,
		int32 MaxTargets
	);

	/**
	 * 부채꼴 범위 검색:
	 * - 먼저 원형 Overlap으로 후보를 모은 뒤
	 * - Forward와의 각도 필터로 콘 범위를 통과시키고
	 * - (선택) LOS 검사
	 */
	static TArray<AActor*> FindActorsInCone(
		UWorld* World,
		const FVector& Origin,
		const FVector& Forward,
		float Radius,
		float HalfAngleDeg,
		const FCollisionObjectQueryParams& ObjectQueryParams,
		ECollisionChannel TraceChannelForLOS,
		const TArray<AActor*>& ActorsToIgnore,
		bool bRequireLineOfSight,
		bool bIgnoreZ,
		int32 MaxTargets
	);
};
