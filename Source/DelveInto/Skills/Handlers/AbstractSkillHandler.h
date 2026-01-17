#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/SkillTypes.h"
#include "AbstractSkillHandler.generated.h"

class USkillData;
class USkillInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillRegistered, ESkillSlot, Designator, const USkillData*, SkillData);

UCLASS(ClassGroup=(DelveInto), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UAbstractSkillHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbstractSkillHandler();

	virtual void BeginPlay() override;

	USkillInstance* GetSkill(ESkillSlot Designator) const;

	/**
	 * 런타임에 Designator에 SkillData를 바인딩(등록)하고 SkillInstance를 생성합니다.
	 * - 무기 교체/로드/리스폰 등에서 재사용 가능한 API.
	 */
	USkillInstance* RegisterSkill(ESkillSlot Designator, USkillData* SkillData, int32 Level = 1);

public:
	UPROPERTY(BlueprintAssignable, Category="Skill")
	FOnSkillRegistered OnSkillRegistered;

protected:
	// 초기 기본 스킬(핫바 등)
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	TMap<ESkillSlot, TObjectPtr<USkillData>> DefaultSkills;

	UPROPERTY(Transient)
	TMap<ESkillSlot, TObjectPtr<USkillInstance>> Skills;
};
