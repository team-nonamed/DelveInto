#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NewSystem/Interfaces/Interactable.h" // 경로 맞춰주세요
#include "ShopNPC.generated.h"

class UShopWidget;

UCLASS()
class DELVEINTO_API AShopNPC : public AActor, public IInteractable
{
	GENERATED_BODY()
    
public:    
	AShopNPC();

	// 상점 UI 위젯 클래스 (블루프린트에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	TSubclassOf<UShopWidget> ShopWidgetClass;

	// IInteractable 인터페이스 구현 (C++ 용)
	virtual void Interact_Implementation(AActor* Interactor) override;
};