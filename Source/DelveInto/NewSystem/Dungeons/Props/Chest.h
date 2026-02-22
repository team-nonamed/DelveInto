#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "NewSystem/Interfaces/Interactable.h"
#include "Chest.generated.h"

class UPaperSpriteComponent;
class UPaperSprite;
class USoundBase; // [추가] 효과음 클래스 전방 선언

UCLASS(Abstract, Blueprintable)
class DELVEINTO_API AChest : public AActor, public IInteractable
{
	GENERATED_BODY()
    
public:    
	AChest();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* ChestSprite;

	// --- 시각 효과 ---
	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	UPaperSprite* ClosedSprite;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	UPaperSprite* OpenedSprite;

	// --- 청각 효과 (SFX) ---
	UPROPERTY(EditAnywhere, Category = "Chest|Audio")
	USoundBase* InteractSound; // 상자가 열릴 때 날 소리 (끼이익-)

	UPROPERTY(EditAnywhere, Category = "Chest|Audio")
	USoundBase* OpenSound; // 상자가 열릴 때 날 소리 (끼이익-)

	// --- 드랍 아이템 ---
	UPROPERTY(EditAnywhere, Category = "Chest|Loot")
	FGameplayTag LootItemID;

	UPROPERTY(EditAnywhere, Category = "Chest|Loot")
	int32 LootAmount = 5;

	bool bIsOpen = false;

public:
	virtual void Interact_Implementation(AActor* Interactor) override;
};