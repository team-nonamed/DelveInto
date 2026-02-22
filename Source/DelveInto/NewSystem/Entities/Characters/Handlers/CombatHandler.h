// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NewSystem/Weapons/WeaponData.h"
#include "NewSystem/Widgets/HandDisplayWidget.h"
#include "CombatHandler.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCombatHandler, Log, All);

class UPerkHandler; // 퍽 핸들러 전방 선언

UCLASS(ClassGroup=(Handler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UCombatHandler : public UActorComponent
{
#pragma region Inner Events
    GENERATED_BODY()
    
public:
    UCombatHandler();

protected:
    virtual void BeginPlay() override;
#pragma endregion

#pragma region Properties
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
    TObjectPtr<UWeaponData> DefaultWeaponData;
    
    UPROPERTY(VisibleInstanceOnly, Category = Runtime)
    TObjectPtr<UWeaponData> WeaponData;

    UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
    TMap<EWeaponSkillSlot, TObjectPtr<USkillBase>> Skills;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Runtime)
    TMap<EWeaponSkillSlot, float> CastSpeeds;

    UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
    TObjectPtr<USkillBase> CurrentActiveSkill = nullptr;

    UPROPERTY(VisibleInstanceOnly, Category = Runtime)
    ESkillState CurrentActiveSkillState;

    UPROPERTY()
    TObjectPtr<UHandDisplayWidget> DisplayWidget;

    // [신규] 퍽 시스템 연동을 위한 핸들러 캐싱
    UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
    TObjectPtr<UPerkHandler> PerkHandler;
#pragma endregion

#pragma region Event Handlers
protected:
    UFUNCTION()
    void HandleDefaultSkillStateTransition(ESkillState SkillState, UPaperFlipbook* Flipbook, bool IsLoopableFlipbook);

    UFUNCTION()
    void HandleSkillDeactivation(USkillBase* OldSkill);

    UFUNCTION()
    void HandleSkillActivation(USkillBase* NewSkill);

    UFUNCTION()
    void HandleSkillHit(USkillBase* InstigatorSkill, AActor* Victim, float BaseDamage);
#pragma endregion

#pragma region Methods
public:
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool EquipWeapon(UWeaponData* NewWeaponData);

    UFUNCTION(BlueprintCallable, Category = "Setup")
    void Initialize(UHandDisplayWidget* InDisplayWidget);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool HandleInput(EWeaponSkillSlot Slot, bool bIsPressed);

    // [신규] 퍽(Perk) 시스템의 연산을 거친 최종 데미지 산출
    UFUNCTION(BlueprintCallable, Category = "Combat")
    float CalculateFinalDamage(float BaseDamage, AActor* Victim, int32 ComboCount, EWeaponSkillSlot AttackSlot);

protected:
    void RegisterHandlers(USkillBase* NewSkill);

    void UnregisterHandlers(USkillBase* OldSkill);
#pragma endregion
};