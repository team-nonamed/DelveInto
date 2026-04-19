#include "DungeonFullMapWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Widgets/Images/MinimapRoomIcon.h"

void UDungeonFullMapWidget::RefreshMapBoard()
{
    if (!DungeonManager || !MapBoard || !RoomIconClass) return;

    MapBoard->ClearChildren();

    // 1. 플레이어의 현재 논리 좌표 (중앙점 기준)
    FIntPoint PlayerCoord = DungeonManager->GetCurrentPlayerCoordinate();

    // 2. [변경점] 5x5 루프가 아니라 DungeonManager의 전체 방 데이터를 순회합니다.
    const TMap<FIntPoint, FRoomStatus>& RoomData = DungeonManager->GetRoomDataMap();

    for (const auto& Pair : RoomData)
    {
        FIntPoint TargetCoord = Pair.Key;
        const FRoomStatus& Status = Pair.Value;

        // 3. [조건] 탐색한 방(방문했거나 위치를 아는 방)만 그립니다.
        if (Status.VisitStatus != ERoomVisitStatus::Unknown)
        {
            UMinimapRoomIcon* NewIcon = CreateWidget<UMinimapRoomIcon>(this, RoomIconClass);
            if (NewIcon && Status.RoomClass)
            {
                if (ARoomBase* CDO = Cast<ARoomBase>(Status.RoomClass->GetDefaultObject()))
                {
                    // 방문 여부에 따른 아이콘 설정
                    if (Status.VisitStatus == ERoomVisitStatus::Unvisited)
                    {
                        NewIcon->SetupIcon(CommonBorderTexture, UnvisitedIconTexture, FullMapIconSize);
                    }
                    else
                    {
                        NewIcon->SetupIcon(CommonBorderTexture, CDO->GetIcon(), FullMapIconSize);
                    }
                }

                UCanvasPanelSlot* CanvasSlot = MapBoard->AddChildToCanvas(NewIcon);
                
                // 4. 상대 좌표 계산 (플레이어 방을 0,0으로 만드는 로직)
                int32 OffsetX = TargetCoord.X - PlayerCoord.X;
                int32 OffsetY = TargetCoord.Y - PlayerCoord.Y;

                float UILocX = OffsetY * FullMapIconSize;
                float UILocY = OffsetX * -FullMapIconSize;

                CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
                CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
                CanvasSlot->SetPosition(FVector2D(UILocX, UILocY));
                CanvasSlot->SetSize(FVector2D(FullMapIconSize, FullMapIconSize));
                CanvasSlot->SetAutoSize(false);
            }
        }
    }
}

void UDungeonFullMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    // 부모의 NativeTick(회전 로직)을 호출하지 않음으로써 지도를 북쪽 고정(North-up)으로 만듭니다.
    // 만약 전체 맵도 회전시키고 싶다면 Super::NativeTick을 호출하세요.
}