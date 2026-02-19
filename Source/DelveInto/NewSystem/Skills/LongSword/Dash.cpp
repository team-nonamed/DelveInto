// Fill out your copyright notice in the Description page of Project Settings.


#include "Dash.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
USkill_LongSword_Dash::USkill_LongSword_Dash()
{
	// 대시는 즉발 스킬이며 콤보가 아님
	ExecutionType = ESkillExecutionType::Immediate;
	bHasCombo = false;
    
	// 대시 도중에 공격 등으로 끊기면 안 되므로 false로 둡니다.
	bIsInterruptable = false;
}

bool USkill_LongSword_Dash::ExecuteSkillLogic_Implementation()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return false;

	FVector DashDirection = FVector::ZeroVector;

	// 1. 현재 플레이어가 키보드(WASD)로 입력 중인 가속도 방향을 가져옵니다.
	FVector CurrentAcceleration = OwnerChar->GetCharacterMovement()->GetCurrentAcceleration();
    
	if (!CurrentAcceleration.IsNearlyZero())
	{
		// 입력 중인 방향으로 대시 (예: A를 누르며 대시하면 왼쪽으로 대시)
		DashDirection = CurrentAcceleration.GetSafeNormal();
	}
	else
	{
		// 2. 가만히 서 있다면 캐릭터가 바라보는 정면 방향으로 대시
		DashDirection = OwnerChar->GetActorForwardVector();
	}

	// 3. 수직(Z축) 대시 제어
	if (!bAllowVerticalDash)
	{
		DashDirection.Z = 0.0f;
		DashDirection.Normalize(); // Z를 0으로 만들었으니 다시 정규화(길이 1) 해줍니다.
	}

	// 4. 최종 속도 벡터 계산
	FVector DashVelocity = DashDirection * DashStrength;

	// 5. 캐릭터 발사 (LaunchCharacter)
	// 뒤의 true, true 인자는 기존에 떨어지거나 걷고 있던 속도를 무시하고 대시 속도로 덮어씌운다는 뜻입니다.
	OwnerChar->LaunchCharacter(DashVelocity, true, true);

	// 로그 출력 (디버깅용)
	UE_LOG(LogSkill, Display, TEXT("%s: 방향 %s 로 대시 시전!"), *SkillName.ToString(), *DashDirection.ToString());

	return true;
}

