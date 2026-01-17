#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Input/CombatInputReceiver.h"
#include "Interfaces/Hurts/HurtHandler.h"
#include "AbstractHost.generated.h"

class UAttackHandler;
class UAbstractSkillHandler;
class UInventoryHandler;

/**
 * Host(논리적 진입점) Actor.
 * - 핸들러 컴포넌트를 소유/연결하고
 * - InputController로부터 전달받은 입력을 Handler로 라우팅
 * - IHurtHandler도 Actor 레벨에서 구현하고 내부 HealthHandler(미구현)로 위임하는 형태를 권장
 */
UCLASS(Abstract)
class DELVEINTO_API AAbstractHost
	: public ACharacter
	, public ICombatInputReceiver
	, public IHurtHandler
{
	GENERATED_BODY()

public:
	AAbstractHost();

	// ICombatInputReceiver
	virtual void HandleSkillInput(ESkillSlot Designator, ETriggerEvent TriggerEvent) override;

	// IHurtHandler (가상함수: HealthHandler로 위임하는 방식 권장)
	virtual FHurtResult HandleHurt(const FHurtRequest& Request) override;

	// 핸들러 접근자
	UAttackHandler* GetAttackHandler() const { return AttackHandler; }
	UAbstractSkillHandler* GetSkillHandler() const { return SkillHandler; }
	UInventoryHandler* GetInventoryHandler() const { return InventoryHandler; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Handlers")
	TObjectPtr<UAttackHandler> AttackHandler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Handlers")
	TObjectPtr<UAbstractSkillHandler> SkillHandler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Handlers")
	TObjectPtr<UInventoryHandler> InventoryHandler;
};
