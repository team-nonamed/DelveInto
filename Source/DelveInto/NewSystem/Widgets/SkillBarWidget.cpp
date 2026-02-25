#include "SkillBarWidget.h"
#include "SkillSlotWidget.h"
#include "NewSystem/Entities/Characters/Handlers/CombatHandler.h"
#include "NewSystem/Skills/SkillBase.h"

void USkillBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 위젯이 생성될 때 런타임 매핑을 위해 내부 맵에 슬롯들을 등록해 둡니다.
    // 프로젝트에서 사용하는 EWeaponSkillSlot Enum 값에 맞춰 매핑하세요.
    SlotMap.Empty();
    
    if (PrimarySlot) 
    {
        SlotMap.Add(EWeaponSkillSlot::SkillQ, PrimarySlot);
    }
    if (SecondarySlot) 
    {
        SlotMap.Add(EWeaponSkillSlot::SkillE, SecondarySlot);
    }
    
    // if (UltimateSlot) SlotMap.Add(EWeaponSkillSlot::Ultimate, UltimateSlot);
}

void USkillBarWidget::InitializeSkillBar(UCombatHandler* InCombatHandler)
{
    if (!InCombatHandler) return;

    // CombatHandler를 기억해 둡니다.
    CachedCombatHandler = InCombatHandler;
    

    InCombatHandler->OnWeaponEquipped.RemoveDynamic(this, &USkillBarWidget::OnWeaponSkillsUpdated);
    InCombatHandler->OnWeaponEquipped.AddDynamic(this, &USkillBarWidget::OnWeaponSkillsUpdated);

    // [수정] 초기화 시점에도 인자 없이 바로 업데이트 함수를 호출합니다.
    OnWeaponSkillsUpdated();
}

void USkillBarWidget::OnWeaponSkillsUpdated()
{
    if (!CachedCombatHandler.IsValid()) return;

    // 1. 신호를 받았으니, CombatHandler에서 현재 스킬 목록을 직접 당겨옵니다.
    TMap<EWeaponSkillSlot, USkillBase*> EquippedSkills = CachedCombatHandler->GetEquippedSkills();

    // 2. 먼저 모든 슬롯의 연결을 초기화(비움)합니다.
    for (auto& Pair : SlotMap)
    {
        if (USkillSlotWidget* SlotWidget = Pair.Value)
        {
            SlotWidget->BindSkill(nullptr);
        }
    }

    // 3. 가져온 스킬들을 맞는 슬롯에 바인딩합니다.
    for (const auto& Pair : EquippedSkills)
    {
        EWeaponSkillSlot SlotType = Pair.Key;
        USkillBase* Skill = Pair.Value;

        if (USkillSlotWidget** FoundSlotWidget = SlotMap.Find(SlotType))
        {
            if (*FoundSlotWidget)
            {
                (*FoundSlotWidget)->BindSkill(Skill);
            }
        }
    }
}