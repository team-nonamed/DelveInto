#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PerkEffectBase.h"
#include "PerkBase.generated.h"

/**
 * 실제 게임 내 존재하는 '퍽' 데이터 에셋입니다.
 * 내부에 여러 개의 PerkEffect를 조립하여 가질 수 있습니다.
 */
UCLASS(BlueprintType, Blueprintable)
class DELVEINTO_API UPerkBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk Info")
	FName PerkName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk Info")
	int32 BaseRarity; // 기본 등장 등급 (0: 일반, 1: 드문, 2: 희귀, 3: 영웅, 4: 전설)

	// BP 디테일 패널에서 Effect를 Add하여 조립할 수 있습니다.
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Effects")
	TArray<TObjectPtr<UPerkEffectBase>> PerkEffects;
};