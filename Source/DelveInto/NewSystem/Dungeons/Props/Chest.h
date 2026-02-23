#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Components/BoxComponent.h"
#include "NewSystem/Billboards/FacingSpriteComponent.h"
#include "NewSystem/Interfaces/Interactable.h"
#include "Chest.generated.h"

USTRUCT(BlueprintType)
struct FLootEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmount = 3;

	// 이 아이템이 나올 확률 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 1.0f;
};

class USoundBase;

UCLASS(Abstract, Blueprintable)
class DELVEINTO_API AChest : public AActor, public IInteractable
{
	GENERATED_BODY()
    
public:    
	AChest();

protected:
	virtual void BeginPlay() override; // [추가] 시작할 때 닫힌 상태로 세팅하기 위함

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UFacingSpriteComponent* ChestSprite;

	// --- 시각 효과 ---
	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	UPaperSprite* ClosedSprite;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	UPaperSprite* OpenedSprite;

	// --- 청각 효과 (SFX) ---
	UPROPERTY(EditAnywhere, Category = "Chest|Audio")
	USoundBase* InteractSound;

	UPROPERTY(EditAnywhere, Category = "Chest|Audio")
	USoundBase* OpenSound;

	// --- 드랍 아이템 ---
	// [수정] 단일 아이템 대신 여러 아이템 리스트를 관리
	UPROPERTY(EditAnywhere, Category = "Chest|Loot")
	TArray<FLootEntry> LootTable;

	// 아이템이 튀어나오는 힘의 세기
	UPROPERTY(EditAnywhere, Category = "Chest|Loot")
	float ExplosionForce = 500.0f;

	bool bIsOpen = false;

	// [신규] 상호작용 레이저를 맞아줄 투명 박스
	// 1. 최상단 루트 (물리적 중심)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* InteractCollision;

	// 2. 피벗 조절용 허브 (박스 대비 위치 오프셋 담당)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

public:
	virtual void Interact_Implementation(AActor* Interactor) override;

	// [신규] 에디터에서 값이 변경될 때마다 실행되는 함수 (블루프린트의 Construction Script 역할)
	virtual void OnConstruction(const FTransform& Transform) override;
};