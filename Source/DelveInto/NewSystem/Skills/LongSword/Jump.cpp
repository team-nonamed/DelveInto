// Skill_Jump.cpp
#include "Jump.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USkill_LongSword_Jump::USkill_LongSword_Jump()
{
	bIsInterruptable = true; // [중요] 점프 중 공격 등 다른 스킬 허용
	bEndCastingManually = true; // [중요] 바닥에 닿을 때 수동으로 끝낼 것임
	ExecutionType = ESkillExecutionType::Immediate;
}

// Called when the game starts
void USkill_LongSword_Jump::BeginPlay()
{
	Super::BeginPlay();

	// 캐릭터의 착지 이벤트를 바인딩하여 점프 스킬 종료 시점을 잡음
	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		OwnerChar->LandedDelegate.AddDynamic(this, &USkill_LongSword_Jump::OnLanded);
	}
}

bool USkill_LongSword_Jump::ExecuteSkillLogic_Implementation()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return false;

	// 이미 공중이라면 점프 실패 (또는 다중 점프 로직 처리)
	if (OwnerChar->GetCharacterMovement()->IsFalling()) return false;

	OwnerChar->Jump();
	return true;
}

void USkill_LongSword_Jump::OnLanded(const FHitResult& Hit)
{
	// 착지했을 때, 현재 점프 스킬이 활성화(Casting 등) 상태라면 종료시킴
	if (bIsActivated && CurrentState == ESkillState::Casting)
	{
		// 바닥에 닿았으므로 PostCasting(착지 모션)으로 넘기거나 바로 Deactivate
		// 착지 애니메이션이 있다면 HandlePostCasting(true); 호출
		DeactivateSkill(); 
	}
}

