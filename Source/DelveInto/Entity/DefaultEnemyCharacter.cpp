// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/DefaultEnemyCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ADefaultEnemyCharacter::ADefaultEnemyCharacter()
{
	// 컨트롤러 회전 사용하지 않음
	bUseControllerRotationYaw = false;
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
		MovementComponent->RotationRate = FRotator(0.f, 540.f, 0.f); // 필요에 따라 조정
	}
	
}

// Called when the game starts or when spawned
void ADefaultEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADefaultEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADefaultEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

