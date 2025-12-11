// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DELVEINTO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** 공격에 사용될 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	UAnimMontage* AttackMontage;

	/** 공격 히트박스로 사용할 콜리전 컴포넌트(캐릭터에 붙어 있는 걸 참조) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	USphereComponent* AttackHitbox;

	/** 현재 공격 중인지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bIsAttacking = false;

	/** 이번 공격에서 이미 맞춘 액터 목록 (중복 타격 방지) */
	TSet<TWeakObjectPtr<AActor>> HitActors;

	/** 캐시해 둔 Owner 캐릭터 */
	TWeakObjectPtr<ACharacter> OwnerCharacter;

public:	
	/** 외부(캐릭터/AI)에서 호출: 공격 시도 */
	void Attack();

	/** AnimNotify 등에서 호출: 히트박스 활성화 */
	void EnableHitbox();

	/** AnimNotify 등에서 호출: 히트박스 비활성화 + 공격 종료 */
	void DisableHitbox();

protected:
	UFUNCTION()
		void OnAttackHitboxBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	/** 내부에서 Owner 캐릭터 반환 (캐시) */
	ACharacter* GetOwnerCharacter() const;
};
