#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "GameFramework/Actor.h"
#include "ItemInstance.generated.h"

class USphereComponent;
class UPaperSpriteComponent;

UCLASS()
class DELVEINTO_API AItemInstance : public AActor
{
	GENERATED_BODY()
    
public:    
	AItemInstance();

	// [추가] 스폰 직후 데이터와 수량을 주입받는 초기화 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitializeItem(UItemData* InItemData, int32 InAmount);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* PickupCollider;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* ItemSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UItemData* ItemData = nullptr; 

	// 땅에 떨어져 있는 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Amount = 1;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};