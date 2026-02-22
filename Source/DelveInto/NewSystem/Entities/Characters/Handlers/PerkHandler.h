#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NewSystem/Weapons/WeaponData.h"
#include "PerkHandler.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPerk, Log, All);

class UPerkBase;

// --- [신규] 스탯 카테고리 열거형 (여기로 이동) ---
UENUM(BlueprintType)
enum class EStatCategory : uint8
{
	MaxHealth,
	CritChance,
	MovementSpeed
};

// 델리게이트 선언부
DECLARE_MULTICAST_DELEGATE_FiveParams(FOnCalculateDamageSignature, AActor* /*Attacker*/, AActor* /*Victim*/, EWeaponSkillSlot /*AttackType*/, int32 /*ComboCount*/, float& /*InOutDamage*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnModifyCastSpeedSignature, EWeaponSkillSlot /*Slot*/, float& /*InOutSpeed*/, AActor* /*Instigator*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDashExecutedSignature, AActor* /*Instigator*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnHitTargetSignature, AActor* /*Attacker*/, AActor* /*Victim*/, float /*FinalDamage*/, EWeaponSkillSlot /*AttackType*/);

// --- [신규] 스탯 변경을 알리는 다이내믹 델리게이트 (블루프린트에서 바인딩하기 위해 Dynamic 채택) ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangedSignature, EStatCategory, StatType, float, DeltaValue);

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
	FOnHitTargetSignature OnHitTarget;

	// --- [신규] 스탯 변경 이벤트 ---
	UPROPERTY(BlueprintAssignable, Category = "Perk|Events")
	FOnStatChangedSignature OnStatChanged;

	UFUNCTION(BlueprintCallable, Category = "Perk")
	void AddPerk(UPerkBase* NewPerk, int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Perk")
	void RemovePerk(UPerkBase* PerkToRemove);
	
	UFUNCTION(BlueprintCallable, Category = "Perk")
	int GetPerkLevel(UPerkBase* Perk);

protected:
	UPROPERTY()
	TMap<TObjectPtr<UPerkBase>, int32> ActivePerks;
};