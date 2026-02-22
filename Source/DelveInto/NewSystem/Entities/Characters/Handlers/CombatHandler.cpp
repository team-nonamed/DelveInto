// Fill out your copyright notice in the Description page of Project Settings.
#include "CombatHandler.h"

#include "PerkHandler.h"
#include "Engine/DamageEvents.h"

DEFINE_LOG_CATEGORY(LogCombatHandler);

UCombatHandler::UCombatHandler()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCombatHandler::BeginPlay()
{
    Super::BeginPlay();

    // 런타임 최적화를 위해 Owner에 부착된 PerkHandler를 1회 탐색 및 캐싱합니다.
    if (AActor* Owner = GetOwner())
    {
        PerkHandler = Owner->FindComponentByClass<UPerkHandler>();
    }
}

void UCombatHandler::HandleDefaultSkillStateTransition(ESkillState SkillState, UPaperFlipbook* Flipbook, bool IsLoopableFlipbook)
{
    if (!DisplayWidget)
    {
       UE_LOG(LogCombatHandler, Error, TEXT("손 동작을 묘사할 Image Widget이 존재하지 않습니다!"));
       return;
    }

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

    if (!NewSkill->OnSkillHit.IsAlreadyBound(this, &UCombatHandler::HandleSkillHit))
    {
       NewSkill->OnSkillHit.AddDynamic(this, &UCombatHandler::HandleSkillHit);
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

    if (!OldSkill->OnSkillDeactivated.IsAlreadyBound(this, &UCombatHandler::HandleSkillDeactivation))
    {
       OldSkill->OnSkillDeactivated.RemoveDynamic(this, &UCombatHandler::HandleSkillDeactivation);
    }
    if (OldSkill->OnSkillHit.IsAlreadyBound(this, &UCombatHandler::HandleSkillHit))
    {
       OldSkill->OnSkillHit.RemoveDynamic(this, &UCombatHandler::HandleSkillHit);
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
          USkillBase* NewSkill = NewObject<USkillBase>(GetOwner(), SkillClass);
          if (NewSkill)
          {
             NewSkill->RegisterComponent();

             RegisterHandlers(NewSkill);

             if (float* CastSpeedPtr = CastSpeeds.Find(Slot))
             {
                NewSkill->CastSpeed = *CastSpeedPtr;
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

    if (DefaultWeaponData && !WeaponData)
    {
       EquipWeapon(DefaultWeaponData);
    }
}

bool UCombatHandler::HandleInput(EWeaponSkillSlot Slot, bool bIsPressed)
{
    TObjectPtr<USkillBase> TargetSkill = Skills.FindRef(Slot);
    if (!TargetSkill) return false;

    // 기본 공격 속도 산출
    float Speed = CastSpeeds.Contains(Slot) ? CastSpeeds[Slot] : 1.0f;

    // [핵심 연동] PerkHandler가 존재할 경우 속도 변조 이벤트를 송출하여 Speed 값을 재산출
    if (PerkHandler)
    {
        PerkHandler->OnModifyCastSpeed.Broadcast(Slot, Speed, GetOwner());
    }

    // 1. 이미 어떤 스킬이 실행 중일 때
    if (CurrentActiveSkill)
    {
       if (CurrentActiveSkill == TargetSkill)
       {
          if (bIsPressed) return CurrentActiveSkill->HandleKeyPressed(Speed);
          else return CurrentActiveSkill->HandleKeyReleased();
       }

       if (CurrentActiveSkill->bIsInterruptable)
       {
          if (bIsPressed)
          {
             CurrentActiveSkill->DeactivateSkill();
             return TargetSkill->HandleKeyPressed(Speed);
          }
          return false;
       }

       if (CurrentActiveSkillState != ESkillState::PostCasting && CurrentActiveSkillState != ESkillState::Idle)
       {
          if (bIsPressed)
          {
             UE_LOG(LogCombatHandler, Display, TEXT("%s 실행 중이라 %s 차단됨."), *CurrentActiveSkill->GetName(), *TargetSkill->GetName());
          }
          return false;
       }
    }

    // 2. 실행 중인 스킬이 없거나 캔슬 가능할 때
    if (bIsPressed)
    {
       return TargetSkill->HandleKeyPressed(Speed);
    }
    else
    {
       return TargetSkill->HandleKeyReleased();
    }
}

float UCombatHandler::CalculateFinalDamage(float BaseDamage, AActor* Victim, FGameplayTag AttackType)
{
    float FinalDamage = BaseDamage;
    
    // [핵심 연동] 공격 대상, 공격 타입(태그)을 기반으로 Perk 시스템에 데미지 변조를 위임
    if (PerkHandler)
    {
        PerkHandler->OnCalculateDamage.Broadcast(GetOwner(), Victim, AttackType, FinalDamage);
    }
    
    return FinalDamage;
}

void UCombatHandler::HandleSkillHit(USkillBase* InstigatorSkill, AActor* Victim, float BaseDamage, FGameplayTag AttackType)
{
   if (!Victim || !GetOwner()) return;

   float FinalDamage = BaseDamage;

   // 1. 퍽(Perk) 시스템 연동하여 데미지 변조
   if (PerkHandler)
   {
      PerkHandler->OnCalculateDamage.Broadcast(GetOwner(), Victim, AttackType, FinalDamage);
   }

   // 2. 최종 산출된 데미지로 실제 타격(TakeDamage) 수행
   FDamageEvent DamageEvent;
   Victim->TakeDamage(FinalDamage, DamageEvent, GetOwner()->GetInstigatorController(), GetOwner());
}