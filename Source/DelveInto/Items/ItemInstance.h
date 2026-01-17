#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ItemInstance.generated.h"

class UAbstractItemDefinition;
class USkillInstance;

/**
 * 런타임 아이템 인스턴스(스택 수량, 기본 공격 스킬 인스턴스 등)
 */
UCLASS(BlueprintType)
class DELVEINTO_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	void InitFromDefinition(UAbstractItemDefinition* InDef);

	UAbstractItemDefinition* GetDefinition() const { return Definition; }
	int32 GetStackCount() const { return StackCount; }

	USkillInstance* GetBasicAttackSkillInst() const { return BasicAttackSkillInst; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<UAbstractItemDefinition> Definition = nullptr;

	UPROPERTY(Transient)
	int32 StackCount = 1;

	// 무기 기본공격도 스킬 인스턴스로 통일
	UPROPERTY(Transient)
	TObjectPtr<USkillInstance> BasicAttackSkillInst = nullptr;
};
