#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class ADelveCharacter;
class UItemData; // [신규] 아이템 데이터 전방 선언

UCLASS()
class DELVEINTO_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 구매자(플레이어) 정보를 캐싱
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void SetBuyer(ADelveCharacter* Player);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	ADelveCharacter* BuyerCharacter;

	// [신규] 판매할 포션의 원본 데이터 (블루프린트에서 설정!)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	UItemData* PotionItemData;

	// 포션 가격
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 PotionPrice = 50; 

	// UMG(블루프린트) 버튼 클릭 시 호출할 C++ 함수
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuyPotion();

	// 상점 닫기 함수
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void CloseShop();
};