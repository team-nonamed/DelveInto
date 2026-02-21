#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "ItemDBRow.h"
#include "ItemDBSubsystem.generated.h"

/**
 * 게임 전역에서 접근 가능한 아이템 데이터베이스 매니저
 */
UCLASS()
class DELVEINTO_API UItemDBSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ItemID를 넣어주면 해당 아이템 정보를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Item DB")
	bool GetItemInfo(FGameplayTag ItemID, FItemDBRow& OutItemInfo) const;

private:
	// O(1) 검색 속도를 위한 해시맵 (Key: ItemID)
	UPROPERTY()
	TMap<FGameplayTag, FItemDBRow> ItemDatabase;
};