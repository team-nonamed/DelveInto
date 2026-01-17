#include "Items/ItemInstance.h"
#include "Items/ItemDefinition.h"
#include "Skills/Instances/SkillInstance.h"
#include "Skills/Data/SkillData.h"

void UItemInstance::InitFromDefinition(UAbstractItemDefinition* InDef)
{
	Definition = InDef;
	StackCount = 1;

	BasicAttackSkillInst = nullptr;

	if (!Definition) return;

	if (USkillData* BaseSkill = Definition->GetBaseAttackSkill())
	{
		// Outer를 ItemInstance로 두면 수명 관리가 깔끔합니다.
		BasicAttackSkillInst = NewObject<USkillInstance>(this);
		BasicAttackSkillInst->Init(BaseSkill, 1);
	}
}
