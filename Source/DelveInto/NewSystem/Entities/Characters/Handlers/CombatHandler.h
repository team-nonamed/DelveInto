// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NewSystem/Weapons/WeaponData.h"
#include "NewSystem/Widgets/HandDisplayWidget.h"
#include "CombatHandler.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCombatHandler, Log, All);

UCLASS(ClassGroup=(Handler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UCombatHandler : public UActorComponent
{
#pragma region Inner Events
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UCombatHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Properties
public:

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
#pragma endregion

#pragma region Event Handlers

protected:
	UFUNCTION()
	void HandleDefaultSkillStateTransition(ESkillState SkillState, UPaperFlipbook* Flipbook, bool IsLoopableFlipbook);

	UFUNCTION()
	void HandleSkillDeactivation(USkillBase* OldSkill);

	UFUNCTION()
	void HandleSkillActivation(USkillBase* NewSkill);
	
	
#pragma endregion

#pragma region Methods
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool EquipWeapon(UWeaponData* NewWeaponData);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialize(UHandDisplayWidget* InDisplayWidget);

	// [신규] 입력 처리 (Character -> CombatHandler -> Skill)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool HandleInput(EWeaponSkillSlot Slot, bool bIsPressed);

protected:
	void RegisterHandlers(USkillBase* NewSkill);

	void UnregisterHandlers(USkillBase* OldSkill);
	
	
#pragma endregion

};
