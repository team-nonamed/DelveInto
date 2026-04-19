#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapRoomIcon.generated.h"

class UImage;

UCLASS()
class DELVEINTO_API UMinimapRoomIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	// 배경 테두리 (모든 방 공통)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BorderImage;

	// 중앙 아이콘 (방마다 다름)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CenterIconImage;

	/** 두 개의 텍스처를 받아 아이콘을 완성합니다. */
	void SetupIcon(UTexture2D* BorderTexture, UTexture2D* CenterTexture, float Size);
};