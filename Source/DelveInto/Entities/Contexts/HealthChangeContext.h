#include "Entities/Components/HealthComponent.h"
#include "Entities/Interactions/HealthInstigation.h"
#include "HealthChangeContext.generated.h"

USTRUCT(BlueprintType)
struct FHealthChangeContext
{
	GENERATED_BODY()

	/**
	 * Attack 수반 여부
	 */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	bool bHasAttack = false;

	/**
	 * 이 체력 변화에 연관된 Attack
	 */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	FHealthInstigation Attack;

	/** 변경 전 체력 값 */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	float OldHealth = 0.f;

	/** 변경 후 체력 값 (적용 예정 값, Start 시점) */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	float NewHealth = 0.f;

	/** 체력 변화량 (NewHealth - OldHealth) */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	float Delta = 0.f;

	/** 피해인지 회복인지 여부 (피해: true, 회복: false 정도로 사용) */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	bool bIsDamage = true;

	/** 이 HealthComponent 인스턴스 */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	TObjectPtr<UHealthComponent> HealthComponent = nullptr;

	/**
	 * 이 이벤트를 취소할지 여부
	 * - Start 이벤트에서 true로 바꾸면 실제 체력 변경을 막을 수 있음
	 */
	UPROPERTY(BlueprintReadWrite, Category="Health")
	bool bCancelled = false;
};
