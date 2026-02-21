// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemEffect.h"
#include "ItemTypes.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

/**
 * Item의 변하지 않는 Static Data를 담는 Asset
 */
UCLASS()
class DELVEINTO_API UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// [핵심] 기존 EItemType을 지우고 GameplayTag로 변경!
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Type")
	FGameplayTag ItemId;

	// 아이템 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Info")
	FText ItemName;

	// 아이템 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Info")
	FText ItemDescription;

	// 인벤토리 UI에 표시할 아이콘 이미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visual")
	UTexture2D* ItemIcon;

	// 상점 매매가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Economy")
	int32 Price = 10;

	// 최대 겹칠 수 있는 개수 (예: 99개)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Config")
	int32 MaxStack = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item|Effects")
	TArray<UItemEffect*> ItemEffects;
};
