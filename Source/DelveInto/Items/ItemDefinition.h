#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/SkillTypes.h"

#include "ItemDefinition.generated.h"

class USkillData;

UCLASS(Abstract, BlueprintType)
class DELVEINTO_API UAbstractItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	FText DefaultName;

	UPROPERTY(EditDefaultsOnly, Category="UI", meta=(MultiLine=true))
	FText DefaultDescription;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TObjectPtr<UTexture2D> Icon = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TMap<ESkillSlot, TObjectPtr<USkillData>> CastableSkills;

	//TODO: 나중에 타입과 속성이 필요할 경우 사용할 것
	// 태그: Type 단일 + Traits 다중
	UPROPERTY(EditDefaultsOnly, Category="Tags", meta=(Categories="Item.Type"))
	FGameplayTag ItemType;
	
	UPROPERTY(EditDefaultsOnly, Category="Tags", meta=(Categories="Item.Trait"))
	FGameplayTagContainer ItemTraits;

public:
	FPrimaryAssetId GetId() const { return GetPrimaryAssetId(); }

	FText GetDefaultName() const { return DefaultName; }
	FText GetDefaultDescription() const { return DefaultDescription; }
	UTexture2D* GetIcon() const { return Icon; }
	
	USkillData* GetBaseAttackSkill() const
	{
		if (const TObjectPtr<USkillData>* Found = CastableSkills.Find(ESkillSlot::BaseAttack))
		{
			return Found->Get();
		}
		return nullptr;
	}

	USkillData* GetSkill(ESkillSlot Designator) const
	{
		if (const auto* Found = CastableSkills.Find(Designator))
		{
			return Found->Get();
		}
		return nullptr;
	}

	const TMap<ESkillSlot, TObjectPtr<USkillData>>& GetCastableSkills() const { return CastableSkills; }

	bool CanCast(USkillData* Skill) const { return Skill && CastableSkills.FindKey(Skill) != nullptr; }

	// FGameplayTag GetItemType() const { return ItemType; }
	// const FGameplayTagContainer& GetItemTraits() const { return ItemTraits; }
};

/**
 * 가장 기본적인 “생성 가능한” 아이템 정의.
 * - 특별한 동작이 없고, Data만 채워서 쓰는 용도.
 * - 이후 Weapon/Armor/Consumable 등으로 더 세분화해도 됨.
 */
UCLASS(BlueprintType)
class DELVEINTO_API UItemDefinition : public UAbstractItemDefinition
{
	GENERATED_BODY()
	// 추가 필드가 없으면 비워도 됩니다.
};