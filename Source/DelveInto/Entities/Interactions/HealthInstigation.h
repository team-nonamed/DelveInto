#pragma once

#include "CoreMinimal.h"
#include "HealthInstigation.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EHealthInstigationType: uint8
{
	Heal,
	Attack_Physical
};

// TODO: 필요한 경우 Instigation 위에 Instigation이라는 상위 개념을 넣고 Instigation과 Heal로 분리

/*
 * TODO: Instigator와 Receiver을 AActor보다 더 구체적인 Class로 변경해야 함
 *	- 최종 피해량과 넉백량을 계산할 수 있는 Method도 구현하면 좋을듯?
 *		- 이건 그냥 Receiver Class에게 책임을 넘기는게 맞는 것 같다
 *		- 넘겨서 설계함
 */

USTRUCT(BlueprintType)
struct FHealthInstigation
{
	GENERATED_BODY()
#pragma region Attributes
#pragma region Involved Entities
	/**
	 * Instigation을 시전한 주체
	 */
	UPROPERTY(BlueprintReadWrite, Category="Instigation | Entity")
	TWeakObjectPtr<AActor> Instigator = nullptr;

	/**
	 * Instigation을 받는 주체
	 */
	UPROPERTY(BlueprintReadWrite, Category="Instigation | Entity")
	TWeakObjectPtr<AActor> Receiver = nullptr;
#pragma endregion

#pragma region Instigation Status
	/**
	 * Instigation의 타입
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instigation | Status")
	EHealthInstigationType InstigationType = EHealthInstigationType::Attack_Physical;

	/**
	 * Instigation의 기본 값
	 *	- 이 값은 Instigator의 현재 상태를 반영한 결과
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Instigation | Status")
	float BaseRate = 0.f;

	/**
	 * Instigation의 기본 넉백량
	 *	- 이 값은 Instigator의 현재 상태를 반영한 결과
	 */
	UPROPERTY(BlueprintReadWrite, Category="Instigation | Status")
	float BaseKnockback = 0.f;
#pragma endregion
#pragma endregion

#pragma region Methods
	/**
	 * Constructor
	 * @param InInstigator Instigation을 시전한 Actor
	 * @param InReceiver  Instigation을 받는 Actor
	 * @param InInstigationType Instigation의 타입
	 * @param InBaseRate Instigator의 현재 상태를 반영한 기본 피해량
	 * @param InBaseKnockback Instigator의 현재 상태를 반영한 기본 넉백량
	 */
	FHealthInstigation(
		TWeakObjectPtr<AActor> InInstigator,
		TWeakObjectPtr<AActor> InReceiver,
		EHealthInstigationType InInstigationType,
		float InBaseRate,
		float InBaseKnockback
		);

	/**
	 * 기본값으로 Instigation Instance를 생성하는 Constructor
	 */
	FHealthInstigation() = default;
#pragma endregion
};
