#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "GameFramework/Actor.h"
#include "ItemInstance.generated.h"

class USphereComponent;
class UPaperSpriteComponent; // Flipbook을 쓰신다면 UPaperFlipbookComponent로 바꾸셔도 무방합니다.

UCLASS()
class DELVEINTO_API AItemInstance : public AActor
{
	GENERATED_BODY()
    
public:    
	AItemInstance();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitializeItem(UItemData* InItemData, int32 InAmount);

protected:
	// 1. 물리 충돌용 콜라이더 (바닥에 튕기는 용도, 작음)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* PhysicsCollider;

	// 2. 획득 판정용 콜라이더 (캐릭터가 닿는 용도, 큼)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* PickupTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* ItemSprite;

	// [복구 필수] 이 아이템이 어떤 데이터를 가졌는지 저장하는 핵심 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UItemData* ItemData = nullptr; 

	// [복구 필수] 이 아이템의 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Amount = 1;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};