// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NewSystem/Skills/SkillBase.h"
#include "HeavySlash.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Skill), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USkill_LongSword_HeavySlash : public USkillBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkill_LongSword_HeavySlash();

public:
	// 기존 SlashRange 대신 아래 두 변수를 사용합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Combat")
	float ForwardDistance = 100.0f; // 내 몸에서 앞으로 얼마나 떨어진 허공을 벨 것인가?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Combat")
	float SlashWidth = 400.0f; // 좌우로 얼마나 넓게 벨 것인가? (가로 길이)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Combat")
	float SlashRadius = 80.0f; // 검기의 두께 (구체의 반지름)

	// 어떤 물체와 부딪힐지 결정 (보통 카메라나 Visibility 채널 사용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Combat")
	TEnumAsByte<ETraceTypeQuery> TraceChannel; 

	// 디버그용 선을 그릴 것인가? (에디터에서 판정 범위 확인할 때 유용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill | Combat")
	bool bDrawDebug = false;

	
protected:
	virtual bool ExecuteSkillLogic_Implementation() override;
};
