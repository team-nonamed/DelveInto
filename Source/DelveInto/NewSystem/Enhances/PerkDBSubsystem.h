#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "PerkDBSubsystem.generated.h"

class UPerkBase;
class UPerkHandler;

// [신규] UI에게 전달할 "선택지 데이터 세트" 구조체
USTRUCT(BlueprintType)
struct FPerkChoiceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Perk")
	TObjectPtr<UPerkBase> Perk = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Perk")
	int32 NextLevel = 1; // 적용될 목표 레벨
};

UCLASS()
class DELVEINTO_API UPerkDBSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "PerkDB")
	UPerkBase* GetPerkById(FGameplayTag PerkId) const;

	// [수정] UPerkBase* 배열 대신, 방금 만든 구조체 배열을 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "PerkDB")
	TArray<FPerkChoiceData> GetRandomPerksForLevelUp(int32 Count, UPerkHandler* PlayerPerkHandler);

protected:
	UPROPERTY()
	TArray<TObjectPtr<UPerkBase>> CachedPerks;

	void LoadAllPerks();
};