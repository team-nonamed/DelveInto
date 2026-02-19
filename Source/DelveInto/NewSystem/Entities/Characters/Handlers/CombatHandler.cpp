// Fill out your copyright notice in the Description page of Project Settings.
#include "CombatHandler.h"

DEFINE_LOG_CATEGORY(LogCombatHandler);

// Sets default values for this component's properties
UCombatHandler::UCombatHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UCombatHandler::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatHandler::HandleDefaultSkillStateTransition(ESkillState SkillState, UPaperFlipbook* Flipbook, bool IsLoopableFlipbook)
{
	if (!DisplayWidget)
	{
		UE_LOG(LogCombatHandler, Error, TEXT("손 동작을 묘사할 Image Widget이 존재하지 않습니다.!"));
		return;
	}

	// [핵심 방어 코드] CurrentActiveSkill이 없으면 기본속도 1.0f로 설정
	float Speed = CurrentActiveSkill ? CurrentActiveSkill->CastSpeed : 1.0f;
	
	UE_LOG(LogCombatHandler, Display, TEXT("%s를 %f의 속도로 재생합니다."), *Flipbook->GetName(), Speed);
	
	DisplayWidget->PlayFlipbook(Flipbook, IsLoopableFlipbook, Speed);
	CurrentActiveSkillState = SkillState;
}


void UCombatHandler::HandleSkillActivation(USkillBase* NewSkill)
{
	if (CurrentActiveSkill && CurrentActiveSkillState != ESkillState::PostCasting)
	{
		UE_LOG(LogCombatHandler, Error, TEXT("현재 Skill(%s)이 완료되지 않았음에도 다른 Skill(%s)이 활성화 되었습니다!"), *CurrentActiveSkill->SkillName.ToString(), *NewSkill->SkillName.ToString())
		NewSkill->DeactivateSkill();
		return;
	}

	CurrentActiveSkill = NewSkill;
}

void UCombatHandler::HandleSkillDeactivation(USkillBase* OldSkill)
{
	if (!DisplayWidget)
	{
		UE_LOG(LogCombatHandler, Error, TEXT("손 동작을 묘사할 Image Widget이 존재하지 않습니다!"));
		return;
	}

	if (!WeaponData)
	{
		UE_LOG(LogCombatHandler, Error, TEXT("현재 무기의 Data가 유효하지 않습니다!"))
		return;
	}

	if (!WeaponData->IdleFlipbook)
	{
		UE_LOG(LogCombatHandler, Error, TEXT("현재 무기의 Idle Flipbook이 존재하지 않습니다!"))
		return;
	}

	DisplayWidget->PlayFlipbook(WeaponData->IdleFlipbook, true, 1.0f);

	CurrentActiveSkill = nullptr;
	CurrentActiveSkillState = ESkillState::Idle;
}

void UCombatHandler::RegisterHandlers(USkillBase* NewSkill)
{
	if (!NewSkill->OnSkillActivated.IsAlreadyBound(this, &UCombatHandler::HandleSkillActivation))
	{
		NewSkill->OnSkillActivated.AddDynamic(this, &UCombatHandler::HandleSkillActivation);
	}
	
	if (!NewSkill->OnPreCharging.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		NewSkill->OnPreCharging.AddDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (!NewSkill->OnCharging.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		NewSkill->OnCharging.AddDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (!NewSkill->OnPostCharging.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		NewSkill->OnPostCharging.AddDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (!NewSkill->OnPreCasting.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		NewSkill->OnPreCasting.AddDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (!NewSkill->OnCasting.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		NewSkill->OnCasting.AddDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (!NewSkill->OnPostCasting.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		NewSkill->OnPostCasting.AddDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}

	if (!NewSkill->OnSkillDeactivated.IsAlreadyBound(this, &UCombatHandler::HandleSkillDeactivation))
	{
		NewSkill->OnSkillDeactivated.AddDynamic(this, &UCombatHandler::HandleSkillDeactivation);
	}
}

void UCombatHandler::UnregisterHandlers(USkillBase* OldSkill)
{
	if (OldSkill->OnSkillActivated.IsAlreadyBound(this, &UCombatHandler::HandleSkillActivation))
	{
		OldSkill->OnSkillActivated.RemoveDynamic(this, &UCombatHandler::HandleSkillActivation);
	}
	
	if (OldSkill->OnPreCharging.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		OldSkill->OnPreCharging.RemoveDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (OldSkill->OnCharging.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		OldSkill->OnCharging.RemoveDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (OldSkill->OnPostCharging.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		OldSkill->OnPostCharging.RemoveDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (OldSkill->OnPreCasting.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		OldSkill->OnPreCasting.RemoveDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (OldSkill->OnCasting.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		OldSkill->OnCasting.RemoveDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}
	
	if (OldSkill->OnPostCasting.IsAlreadyBound(this, &UCombatHandler::HandleDefaultSkillStateTransition))
	{
		OldSkill->OnPostCasting.RemoveDynamic(this, &UCombatHandler::HandleDefaultSkillStateTransition);
	}

	if (OldSkill->OnSkillDeactivated.IsAlreadyBound(this, &UCombatHandler::HandleSkillDeactivation))
	{
		OldSkill->OnSkillDeactivated.RemoveDynamic(this, &UCombatHandler::HandleSkillDeactivation);
	}
}


bool UCombatHandler::EquipWeapon(UWeaponData* NewWeaponData)
{
	if (!NewWeaponData)
	{
		UE_LOG(LogCombatHandler, Error, TEXT("유효하지 않은 무기를 장착하려고 했습니다."))
		return false;
	}

	if (!GetOwner())
	{
		UE_LOG(LogCombatHandler, Error, TEXT("Owner가 존재하지 않습니다."))
		return false;
	}

	if (CurrentActiveSkill)
	{
		CurrentActiveSkill->DeactivateSkill();
		CurrentActiveSkill = nullptr;
	}

	for (auto& Pair : Skills)
	{
		USkillBase* Skill = Pair.Value;

		if (Skill)
		{
			UnregisterHandlers(Skill);
			Skill->DestroyComponent();
		}
	}

	Skills.Empty();

	WeaponData = NewWeaponData;

	for (const auto& Pair : WeaponData->SkillClasses)
	{
		EWeaponSkillSlot Slot = Pair.Key;
		TSubclassOf<USkillBase> SkillClass = Pair.Value;

		if (SkillClass)
		{
			// Owner를 Character로 설정하여 생성
			USkillBase* NewSkill = NewObject<USkillBase>(GetOwner(), SkillClass);
			if (NewSkill)
			{
				NewSkill->RegisterComponent(); // 컴포넌트 등록 필수

				RegisterHandlers(NewSkill);

				if (float* Speed = CastSpeeds.Find(Slot))
				{
					NewSkill->CastSpeed = *Speed;
				}
				else
				{
					NewSkill->CastSpeed = WeaponData->BaseAttackSpeed;
				}
				
				Skills.Add(Slot, NewSkill);
			}
		}
	}

	HandleSkillDeactivation(nullptr);

	return true;
}

void UCombatHandler::Initialize(UHandDisplayWidget* InDisplayWidget)
{
	if (!InDisplayWidget)
	{
		UE_LOG(LogCombatHandler, Error, TEXT("CombatHandler::Initialize - 유효하지 않은 위젯입니다."));
		return;
	}

	DisplayWidget = InDisplayWidget;

	// 기본 무기가 있고, 아직 무기가 장착되지 않았다면 장착
	if (DefaultWeaponData && !WeaponData)
	{
		EquipWeapon(DefaultWeaponData);
	}
}

bool UCombatHandler::HandleInput(EWeaponSkillSlot Slot, bool bIsPressed)
{
	TObjectPtr<USkillBase> TargetSkill = Skills.FindRef(Slot);
	if (!TargetSkill) return false;

	// 1. 이미 어떤 스킬이 실행 중일 때
	if (CurrentActiveSkill)
	{
		// A. 실행 중인 스킬과 새로 입력된 스킬이 같은 경우 (예: 차징을 떼거나 연타)
		if (CurrentActiveSkill == TargetSkill)
		{
			if (bIsPressed) return CurrentActiveSkill->HandleKeyPressed(CastSpeeds.Contains(Slot) ? CastSpeeds[Slot] : 1.0f);
			else return CurrentActiveSkill->HandleKeyReleased();
		}

		// B. 다른 스킬이 실행 중인데, 현재 스킬이 방해 가능(Interruptable)한 경우 (예: 점프 중 공격)
		if (CurrentActiveSkill->bIsInterruptable)
		{
			if (bIsPressed)
			{
				// [핵심] 기존 스킬을 강제로 끄고 새 스킬을 켭니다.
				CurrentActiveSkill->DeactivateSkill();
				// (이때 HandleSkillDeactivation이 호출되어 CurrentActiveSkill은 nullptr이 됨)
                
				// 타겟 스킬 실행
				return TargetSkill->HandleKeyPressed(CastSpeeds.Contains(Slot) ? CastSpeeds[Slot] : 1.0f);
			}
			return false; // 방해 가능한 스킬이 실행 중인데 다른 키를 '뗐을' 때는 아무것도 안 함
		}

		// C. 현재 스킬이 방해 불가(Lock)인 경우
		// (단, Idle이나 PostCasting 등 스킬이 끝나가는 시점에는 캔슬 허용)
		if (CurrentActiveSkillState != ESkillState::PostCasting && CurrentActiveSkillState != ESkillState::Idle)
		{
			if (bIsPressed)
			{
				UE_LOG(LogCombatHandler, Display, TEXT("%s 실행 중이라 %s 차단됨."), *CurrentActiveSkill->GetName(), *TargetSkill->GetName());
			}
			return false;
		}
	}

	// 2. 실행 중인 스킬이 없거나, PostCasting이라 캔슬 가능할 때 -> 그냥 실행
	if (bIsPressed)
	{
		float Speed = CastSpeeds.Contains(Slot) ? CastSpeeds[Slot] : 1.0f;
		return TargetSkill->HandleKeyPressed(Speed);
	}
	else
	{
		return TargetSkill->HandleKeyReleased();
	}
}
