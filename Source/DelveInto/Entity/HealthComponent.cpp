// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/HealthComponent.h"

DEFINE_LOG_CATEGORY(LogHealth)

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bInitWithMaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
	
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::ApplyDamage);
	}
}

/**
 * 외부에서 입은 Damage를 내부적으로 처리하는 함수
 * @param Victim Damage를 입은 Actor
 * @param Damage 입은 Damage 양
 * @param DamageType 입은 Damage의 타입
 * @param InstigatedBy Damage를 유발한 Controller
 * @param Attacker Damage를 가한 Actor
 */
void UHealthComponent::ApplyDamage(
	AActor* Victim,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* Attacker
	)
{
	if (Damage <= 0.f || IsDead())
	{
		return;
	}

	float const NewHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	SetHealth(NewHealth);
}

/**
 * 체력 값을 설정하는 내부 함수
 * @param NewHealth 새로운 체력 값
 */
void UHealthComponent::SetHealth(float NewHealth)
{
	
	if (FMath::IsNearlyEqual(CurrentHealth, NewHealth))
	{
		return;
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (bDebugHealthLog)
	{
		AActor* Owner = GetOwner();
		const FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");
		UE_LOG(LogHealth, Log,
			TEXT("[HealthComponent] %s: %.1f -> %.1f / Max=%.1f"),
			*OwnerName, CurrentHealth, NewHealth, MaxHealth);
	}
#endif
	
	CurrentHealth = NewHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::TakeDamage(float Amount)
{
	if (Amount <= 0.f || IsDead())
	{
		return;
	}

	float NewHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth);
	SetHealth(NewHealth);
}

void UHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.f || IsDead())
	{
		return;
	}

	float NewHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
	SetHealth(NewHealth);
}
