#pragma once

#include "CoreMinimal.h"
#include "Widgets/DungeonMinimapWidget.h"
#include "DungeonFullMapWidget.generated.h"

UCLASS()
class DELVEINTO_API UDungeonFullMapWidget : public UDungeonMinimapWidget
{
	GENERATED_BODY()

public:
	// 전체 맵은 보통 회전하지 않으므로 Tick에서 회전 로직을 막거나 무시합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	/** 미니맵의 5x5 로직을 덮어써서 전체 맵 로직으로 구현 */
	virtual void RefreshMapBoard() override;

	/** 전체 맵에서 사용할 아이콘 크기 (미니맵보다 작게 설정 가능) */
	UPROPERTY(EditDefaultsOnly, Category = "FullMap Settings")
	float FullMapIconSize = 40.0f;
};