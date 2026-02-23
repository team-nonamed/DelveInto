#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NewSystem/Interfaces/Interactable.h"
#include "ShopNPC.generated.h"

class UShopWidget;
class UBoxComponent;
class USceneComponent;
class UFacingSpriteComponent; // [수정] FacingSprite 전방 선언
class UPaperSprite;           // [수정] Sprite 전방 선언

UCLASS()
class DELVEINTO_API AShopNPC : public AActor, public IInteractable
{
	GENERATED_BODY()
    
public:    
	AShopNPC();

protected:
	// --- 계층 구조 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* InteractCollision;

	// [수정] FacingFlipbook 대신 FacingSprite 사용
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UFacingSpriteComponent* NPCVisual;

	// --- 데이터 ---
	// [수정] Flipbook 대신 Sprite 데이터 사용
	UPROPERTY(EditAnywhere, Category = "Shop|Visual")
	UPaperSprite* IdleSprite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	TSubclassOf<UShopWidget> ShopWidgetClass;

	UPROPERTY()
	TObjectPtr<UShopWidget> ActiveShopWidget;

public:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Interact_Implementation(AActor* Interactor) override;
};