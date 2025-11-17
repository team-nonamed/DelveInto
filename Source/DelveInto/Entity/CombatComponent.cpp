// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/CombatComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (AttackHitbox)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttackHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
		AttackHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		AttackHitbox->OnComponentBeginOverlap.AddDynamic(
			this,
			&UCombatComponent::OnAttackHitboxBeginOverlap
		);
	}
}

ACharacter* UCombatComponent::GetOwnerCharacter() const
{
	return OwnerCharacter.IsValid() ? OwnerCharacter.Get() : nullptr;
}

void UCombatComponent::Attack()
{
	if (bIsAttacking)
	{
		// 나중에 콤보를 넣고 싶으면 여기에서 다음 공격 예약 등 처리
		return;
	}

	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !AttackMontage) return;

	bIsAttacking = true;
	HitActors.Empty();

	OwnerChar->PlayAnimMontage(AttackMontage);
}

void UCombatComponent::EnableHitbox()
{
	if (AttackHitbox)
	{
		HitActors.Empty();
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void UCombatComponent::DisableHitbox()
{
	if (AttackHitbox)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bIsAttacking = false;
}

void UCombatComponent::OnAttackHitboxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor || OtherActor == GetOwner()) return;

	if (HitActors.Contains(OtherActor)) return; // 같은 스윙에서 중복 히트 방지

	// TODO: 팀/태그/인터페이스 등으로 “적”인지 필터링
	if (!OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		return;
	}

	HitActors.Add(OtherActor);

	constexpr float DamageAmount = 20.f;
	UGameplayStatics::ApplyDamage(
		OtherActor,
		DamageAmount,
		GetOwnerCharacter() ? GetOwnerCharacter()->GetController() : nullptr,
		GetOwner(),
		nullptr
	);
}