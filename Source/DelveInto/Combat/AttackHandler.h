#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputTriggers.h"              // ETriggerEvent
#include "Types/SkillTypes.h"

#include "AttackHandler.generated.h"

class UAbstractSkillHandler;
class UInventoryHandler;
class USkillInstance;
class USkillData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillCooldownStarted, ESkillSlot, Designator, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillCooldownEnded, ESkillSlot, Designator);

/**
 * “스킬 실행”을 담당하는 Handler.
 * - 입력 이벤트를 SkillInstance로 전달(상태 전이)
 * - SkillData.Exec 기반으로 월드 액션 수행(근접/투사체/소환)
 * - 성공 시 CommitActivate로 쿨다운 확정
 */
UCLASS(ClassGroup=(DelveInto), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UAttackHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackHandler();

	virtual void BeginPlay() override;

	/**
	 * InputController/Host로부터 “의미론적 슬롯 + TriggerEvent”를 전달받는 엔트리.
	 * - BasicAttack은 Inventory(장착 무기)에서 가져오고
	 * - 나머지는 SkillHandler에서 가져옵니다.
	 */
	bool HandleInput(ESkillSlot Designator, ETriggerEvent TriggerEvent);

public:
	UPROPERTY(BlueprintAssignable, Category="Combat|Cooldown")
	FOnSkillCooldownStarted OnSkillCooldownStarted;

	UPROPERTY(BlueprintAssignable, Category="Combat|Cooldown")
	FOnSkillCooldownEnded OnSkillCooldownEnded;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UAbstractSkillHandler> SkillHandler = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryHandler> InventoryHandler = nullptr;

	// 실행부
	bool TryActivateSkill(ESkillSlot Designator, USkillInstance* Skill, ETriggerEvent TriggerEvent);
	bool ExecuteExecSpec(ESkillSlot Designator, USkillInstance* Skill, const FSkillExecSpec& Exec, const FSkillContext& Ctx);

	bool ExecuteMelee(const FSkillExecSpec& Exec, float Damage, const FSkillContext& Ctx);
	bool ExecuteProjectile(const FSkillExecSpec& Exec, float Damage, const FSkillContext& Ctx);
	bool ExecuteSummon(const FSkillExecSpec& Exec, const FSkillContext& Ctx);

	// 쿨다운 종료 알림을 위한 타이머
	TMap<ESkillSlot, FTimerHandle> CooldownTimers;

	void NotifyCooldownEnded(ESkillSlot Designator);

	FSkillContext BuildContext() const;
};
