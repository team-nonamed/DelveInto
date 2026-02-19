// Fill out your copyright notice in the Description page of Project Settings.


#include "AlterAttack.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
USkill_LongSword_AlterAttack::USkill_LongSword_AlterAttack()
{
	bIsChargingSkill = true;
	bHasVariableDamage = true;
	ExecutionType = ESkillExecutionType::ReleaseAfterHold;
	bHasCombo = false;
}

bool USkill_LongSword_AlterAttack::ExecuteSkillLogic_Implementation()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogSkill, Error, TEXT("%s가 올바르지 않은 Owner에서 호출되었습니다."), *SkillName.ToString())
		return false;
	}

	if (!SwordWaveClass)
	{
		UE_LOG(LogSkill, Error, TEXT("%s에 올바른 발사체가 할당되지 않았습니다."), *SkillName.ToString())
		return false;
	}

	// 3. 투사체(검기) 발사 로직
	if (SwordWaveClass)
	{
		// 카메라 앞 방향으로 발사 위치 설정
		UCameraComponent* Camera = OwnerCharacter->FindComponentByClass<UCameraComponent>();
		if (Camera)
		{
			FVector SpawnLocation = Camera->GetComponentLocation() + (Camera->GetForwardVector() * 100.0f);
			FRotator SpawnRotation = Camera->GetComponentRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerCharacter;
			SpawnParams.Instigator = OwnerCharacter;

			ADelveProjectile* Projectile = GetWorld()->SpawnActor<ADelveProjectile>(SwordWaveClass, SpawnLocation, SpawnRotation, SpawnParams);
            
			if (Projectile)
			{
				// 차징된 비율을 투사체에 전달하여 데미지나 크기, 속도를 키움
				Projectile->InitializeChargeStats(ChargingSeconds / MaxChargingSeconds);
			}
		}
	}

	return true; // 성공적으로 발사됨
}

float USkill_LongSword_AlterAttack::CalculateDamage() const
{
	const float ChargingRatio = ChargingSeconds / MaxChargingSeconds;

	const float FinalDamage = BaseDamage + (MaxDamage - BaseDamage) * ChargingRatio;

	return FinalDamage;
}