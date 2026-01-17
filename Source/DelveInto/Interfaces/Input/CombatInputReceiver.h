#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/SkillTypes.h"
#include "InputTriggers.h"
#include "CombatInputReceiver.generated.h"

UINTERFACE()
class DELVEINTO_API UCombatInputReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 요청대로 UFUNCTION 없이 “가상함수”만.
 * PlayerInputController는 Possessed Pawn에게 이 인터페이스를 통해 입력을 전달합니다.
 */
class DELVEINTO_API ICombatInputReceiver
{
	GENERATED_BODY()

public:
	virtual void HandleSkillInput(ESkillSlot Designator, ETriggerEvent TriggerEvent) = 0;
};
