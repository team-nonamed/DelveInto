#include "Skills/Handlers/AbstractSkillHandler.h"
#include "Skills/Data/SkillData.h"
#include "Skills/Instances/SkillInstance.h"

UAbstractSkillHandler::UAbstractSkillHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAbstractSkillHandler::BeginPlay()
{
	Super::BeginPlay();

	// DefaultSkills로부터 인스턴스 생성
	for (const auto& Pair : DefaultSkills)
	{
		RegisterSkill(Pair.Key, Pair.Value, 1);
	}
}

USkillInstance* UAbstractSkillHandler::GetSkill(ESkillSlot Designator) const
{
	if (const TObjectPtr<USkillInstance>* Found = Skills.Find(Designator))
	{
		return Found->Get();
	}
	return nullptr;
}

USkillInstance* UAbstractSkillHandler::RegisterSkill(ESkillSlot Designator, USkillData* SkillData, int32 Level)
{
	if (!SkillData)
	{
		Skills.Remove(Designator);
		return nullptr;
	}

	USkillInstance* Inst = NewObject<USkillInstance>(this);
	Inst->Init(SkillData, Level);

	Skills.Add(Designator, Inst);
	OnSkillRegistered.Broadcast(Designator, SkillData);

	return Inst;
}
