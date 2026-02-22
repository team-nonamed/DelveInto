#include "CastSpeedModifier.h"
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"

void UPerkEffect_CastSpeedModifier::OnApplied(UPerkHandler* Handler, int32 PerkLevel)
{
	CurrentLevel = PerkLevel;

	// 중복 바인딩 방지 청소
	Handler->OnModifyCastSpeed.RemoveAll(this);

	// 공속 변조 델리게이트에 바인딩!
	Handler->OnModifyCastSpeed.AddUObject(this, &UPerkEffect_CastSpeedModifier::ModifySpeed);
}

void UPerkEffect_CastSpeedModifier::OnRemoved(UPerkHandler* Handler)
{
	Handler->OnModifyCastSpeed.RemoveAll(this);
}

void UPerkEffect_CastSpeedModifier::ModifySpeed(EWeaponSkillSlot Slot, float& InOutSpeed, AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("--- [PerkEffect] 공격 속도 연산 진입 ---"));
	UE_LOG(LogTemp, Warning, TEXT("원본 공속: %f | 슬롯: %d"), InOutSpeed, (int32)Slot);

	// 1. 슬롯 검사 (TargetSlots가 비어있으면 모든 공격 속도 증가, 들어있으면 해당 슬롯만 증가)
	if (!TargetSlots.IsEmpty() && !TargetSlots.Contains(Slot))
	{
		UE_LOG(LogTemp, Warning, TEXT("-> 적용 실패: 타겟 슬롯 불일치"));
		return;
	}

	// 2. 증폭치 가져오기 (예: 배열에 0.2가 들어있으면 20% 공속 증가)
	float BonusValue = GetValueForLevel(CurrentLevel);
	UE_LOG(LogTemp, Warning, TEXT("-> 조건 통과! 적용될 공속 배율: %f"), BonusValue);

	// 3. 연산
	InOutSpeed *= (1.0f + BonusValue);
	UE_LOG(LogTemp, Warning, TEXT("-> 퍽 연산 완료! 최종 공격 속도: %f"), InOutSpeed);
}