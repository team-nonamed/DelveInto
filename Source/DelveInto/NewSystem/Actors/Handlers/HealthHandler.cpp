// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthHandler.h"

#include "GameFramework/Character.h"

UHealthHandler::UHealthHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UHealthHandler::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

float UHealthHandler::ApplyDamage(float DamageAmount)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(CurrentHealth, MaxHealth);
	}

	UE_LOG(LogTemp, Warning, TEXT("Player Hit! Current HP: %f"), CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;

		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

		if (OwnerCharacter && OnDeath.IsBound())
		{
			OnDeath.Broadcast(OwnerCharacter);
		}
	}

	return DamageAmount;
}

void UHealthHandler::OnRegister()
{
	Super::OnRegister();

	// 1. 주인이 있는지 확인
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	// 2. 주인이 ARoomBase(혹은 그 자식)인지 확인
	// IsA는 상속 관계까지 체크해주므로 가장 정확합니다.
	if (!MyOwner->IsA(ACharacter::StaticClass()))
	{
		// [에러 처리] RoomBase가 아니라면?
        
#if WITH_EDITOR
		// 에디터라면 빨간색 로그를 띄워서 개발자에게 경고
		FString ErrorMsg = FString::Printf(TEXT("ERROR: 'SpawnTriggerBox' must be attached to 'ACharacter'! Current Owner: %s"), *MyOwner->GetName());
        
		// 화면에 로그 출력 (Key를 지정해서 도배 방지)
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(reinterpret_cast<uint64>(this), 10.0f, FColor::Red, ErrorMsg);
		}
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
#endif

		PrimaryComponentTick.bCanEverTick = false;
	}
}