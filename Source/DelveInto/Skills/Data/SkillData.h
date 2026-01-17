#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/SkillTypes.h"
#include "SkillData.generated.h"

/**
 * 런타임 이전에 결정되는 Skill의 정적 스펙.
 * - Designator(슬롯 의미론)는 SkillHandler가 관리하므로 여기에 넣지 않습니다.
 * - “무엇을(스펙)”만 정의하고 “어디에 바인딩되는가(슬롯)”는 외부가 결정합니다.
 */
UCLASS(BlueprintType)
class DELVEINTO_API USkillData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 표시용(선택)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	FText Description;

	/**
	 * Skill 분류:
	 * - Skill.Type.Attack / Skill.Type.Summon 같은 “주 타입”은 단일 태그로 두는 것을 권장.
	 * - Traits는 다중 태그(속성)로.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tags", meta=(Categories="Skill.Type"))
	FGameplayTag SkillType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tags", meta=(Categories="Skill.Trait"))
	FGameplayTagContainer SkillTraits;

	// 실행 스펙(정적)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Exec")
	FSkillExecSpec Exec;

public:
	FGameplayTag GetSkillType() const { return SkillType; }
	const FGameplayTagContainer& GetSkillTraits() const { return SkillTraits; }
	const FSkillExecSpec& GetExec() const { return Exec; }
};
