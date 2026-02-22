#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PerkEffectBase.h"
#include "Rarity.h"
#include "Engine/DataAsset.h"
#include "PerkBase.generated.h"

class UTexture2D;

/**
 * 퍽의 성장(Level) 단계별 UI 데이터를 정의합니다.
 */
USTRUCT(BlueprintType)
struct FPerkLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTexture2D> LevelIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText LevelDescription; // 예: "피해량이 10% 증가합니다." (레벨마다 달라짐)
};


/**
 * 게임 내 존재하는 개별 퍽의 데이터 에셋 컨테이너입니다.
 */
UCLASS(BlueprintType, Blueprintable)
class DELVEINTO_API UPerkBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- 기본 식별 정보 ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk Info")
	FText PerkName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk Info")
	FText PerkDescription;
	
	// [신규] 이 퍽의 고유 식별자 (예: Perk.Attack.WarriorBasic)
	// 세이브/로드, DB 검색, 멀티플레이어 동기화 시 Key 값으로 사용됩니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk Info")
	FGameplayTag PerkId;

	// [변경점] 태생 희귀도 (Enum 사용으로 메모리 최소화)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk Info")
	ERarity Rarity = ERarity::Common;

	// --- 성장(Level) 데이터 ---
	// 최대 성장 가능한 레벨 한계
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Growth", meta = (ClampMin = "1"))
	int32 MaxLevel = 1;

	// 레벨별 UI 데이터 (배열 인덱스 0 = 레벨 1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Growth|UI")
	TArray<FPerkLevelData> LevelData;

	// --- 기능 모듈 ---
	// 퍽 획득/레벨업 시 동작할 실제 로직 모듈 배열
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Effects")
	TArray<TObjectPtr<UPerkEffectBase>> PerkEffects;

	// 안전한 레벨 데이터 접근 유틸리티
	UFUNCTION(BlueprintCallable, Category = "Perk")
	bool GetLevelData(int32 Level, FPerkLevelData& OutData) const;
};