#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dungeons/DungeonManager.h"
#include "DungeonMinimapWidget.generated.h"

class UCanvasPanel;
class UOverlay;
class UMinimapRoomIcon;

UCLASS(Abstract)
class DELVEINTO_API UDungeonMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 던전 매니저와 연결하기 위한 초기화 함수
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void InitializeMinimap(ADungeonManager* InDungeonManager);

protected:
	// =========================================================
	// UMG 바인딩 (블루프린트 위젯 이름과 일치해야 함)
	// =========================================================

	/** 맵 전체가 빙글빙글 도는 축 (Pivot 0.5, 0.5 필수) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> MapRotationLayer;

	/** 실제 방 아이콘들이 배치되는 판 (이 녀석의 Translation을 움직여서 이동) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MapBoard;

	// =========================================================
	// 설정 변수
	// =========================================================

	/** 모든 방에 공통으로 사용할 테두리 텍스처 (에디터에서 지정) */
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
	TObjectPtr<UTexture2D> CommonBorderTexture;
	
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
	TObjectPtr<UTexture2D> UnvisitedIconTexture;
	
	/** 방 아이콘 위젯 클래스 (WBP_RoomIcon) */
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
	TSubclassOf<UMinimapRoomIcon> RoomIconClass;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
	float RoomIconSize = 60.0f;
	// =========================================================
	// 내부 로직
	// =========================================================

	UPROPERTY()
	TObjectPtr<ADungeonManager> DungeonManager;

	UFUNCTION()
	virtual void RefreshMapBoard();
};