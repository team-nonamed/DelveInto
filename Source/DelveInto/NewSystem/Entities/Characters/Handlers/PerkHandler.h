#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NewSystem/Weapons/WeaponData.h" // EWeaponSkillSlot 사용을 위해
#include "PerkHandler.generated.h"

class UPerkBase;

// 참조형(&) 파라미터를 사용하여 이펙트에서 직접 값을 수정할 수 있도록 멀티캐스트 델리게이트 선언
DECLARE_MULTICAST_DELEGATE_FourParams(FOnCalculateDamageSignature, AActor* /*Attacker*/, AActor* /*Victim*/, FGameplayTag /*AttackType*/, float& /*InOutDamage*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnModifyCastSpeedSignature, EWeaponSkillSlot /*Slot*/, float& /*InOutSpeed*/, AActor* /*Instigator*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDashExecutedSignature, AActor* /*Instigator*/);

UCLASS(ClassGroup=(Handler), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UPerkHandler : public UActorComponent
{
	GENERATED_BODY()

public:    
	UPerkHandler();

	// --- 전투 시스템 연동용 이벤트 델리게이트 ---
	FOnCalculateDamageSignature OnCalculateDamage;
	FOnModifyCastSpeedSignature OnModifyCastSpeed;
	FOnDashExecutedSignature OnDashExecuted;

	// --- 퍽 관리 API ---
	UFUNCTION(BlueprintCallable, Category = "Perk")
	void AddPerk(UPerkBase* NewPerk, int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Perk")
	void RemovePerk(UPerkBase* PerkToRemove);

protected:
	// 활성화된 퍽과 그 레벨을 추적
	UPROPERTY()
	TMap<TObjectPtr<UPerkBase>, int32> ActivePerks;
};