#include "Entities/AbstractHost.h"

#include "Skills/Handlers/AbstractSkillHandler.h"
#include "Items/InventoryHandler.h"
#include "NewSystem/Entities/Characters/Handlers/CombatHandler.h"

AAbstractHost::AAbstractHost()
{
	// Host는 핸들러들을 “기본 서브오브젝트”로 소유하는 것을 권장
	InventoryHandler = CreateDefaultSubobject<UInventoryHandler>(TEXT("InventoryHandler"));
	SkillHandler     = CreateDefaultSubobject<UAbstractSkillHandler>(TEXT("SkillHandler"));
	AttackHandler    = CreateDefaultSubobject<UCombatHandler>(TEXT("AttackHandler"));
}

void AAbstractHost::HandleSkillInput(ESkillSlot Designator, ETriggerEvent TriggerEvent)
{
	if (AttackHandler)
	{
	}
}

FHurtResult AAbstractHost::HandleHurt(const FHurtRequest& Request)
{
	// 여기서는 최소 구현만 둡니다.
	// 실제로는 HealthHandler 컴포넌트로 위임하는 것을 권장:
	// return HealthHandler->HandleHurt(Request);

	FHurtResult R;
	R.bSuccess = true;
	R.AppliedDamage = Request.Damage;
	return R;
}
