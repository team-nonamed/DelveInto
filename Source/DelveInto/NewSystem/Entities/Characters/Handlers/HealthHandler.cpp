// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthHandler.h"
#include "GameFramework/Character.h" // [필수] ACharacter 캐스팅을 위해 헤더 포함

UHealthHandler::UHealthHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthHandler::OnRegister()
{
	Super::OnRegister();

	// 1. 주인이 있는지 확인
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	// 2. 주인이 ACharacter인지 확인
	if (!MyOwner->IsA(ACharacter::StaticClass()))
	{
#if WITH_EDITOR
		// [수정] 로그 메시지에서 컴포넌트 이름 수정 (SpawnTriggerBox -> HealthHandler)
		FString ErrorMsg = FString::Printf(TEXT("ERROR: 'HealthHandler' must be attached to 'ACharacter'! Current Owner: %s"), *MyOwner->GetName());
        
		if (GEngine)
		{
			// reinterpret_cast 대신 C 스타일 캐스팅이나 GetUniqueID() 사용 가능
			GEngine->AddOnScreenDebugMessage(reinterpret_cast<uint64>(this), 10.0f, FColor::Red, ErrorMsg);
		}
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
#endif
		// 잘못 붙였으면 기능 정지
		PrimaryComponentTick.bCanEverTick = false;
	}
}

void UHealthHandler::BeginPlay()
{
	Super::BeginPlay();

	// 시작 시 체력 초기화
	CurrentHealth = MaxHealth;
    
	// UI 초기화를 위해 시작하자마자 한 번 방송해주면 좋습니다.
	if (OnDamaged.IsBound())
	{
		OnDamaged.Broadcast(MaxHealth, CurrentHealth);
	}
}

float UHealthHandler::ApplyDamage(float DamageAmount)
{
	// 이미 죽었으면 데미지 무시
	if (bIsDead)
	{
		return 0.0f;
	}
	// 체력 감소 및 클램핑
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	// 체력 변경 이벤트 방송
	if (OnDamaged.IsBound())
	{
		OnDamaged.Broadcast(MaxHealth, CurrentHealth);
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] Hit! Current HP: %f"), *GetOwner()->GetName(), CurrentHealth);

	// 사망 체크
	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;

		// [해결] 델리게이트가 ACharacter*를 원하므로 Owner를 캐스팅해서 넘겨줍니다.
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast(OwnerCharacter);
		}
	}

	// [수정] 적용된 데미지(혹은 0)를 반환해야 합니다.
	return DamageAmount;
}

float UHealthHandler::ApplyHeal(float HealAmount)
{
	if (bIsDead || HealAmount <= 0.0f) return 0.0f;
    
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
    
	if (OnHealed.IsBound())
	{
		OnHealed.Broadcast(MaxHealth, CurrentHealth);
	}
	return HealAmount;
}