#include "DungeonMinimapWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Images/MinimapRoomIcon.h" // 경로가 맞는지 확인해 주세요.

void UDungeonMinimapWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // =========================================================================
    // [1번 방법 적용] 위젯이 생성될 때 UGameplayStatics를 이용해 매니저를 스스로 찾습니다.
    // =========================================================================
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonManager::StaticClass());
    if (ADungeonManager* FoundManager = Cast<ADungeonManager>(FoundActor))
    {
        // 매니저를 성공적으로 찾았다면 내장된 초기화 함수를 호출합니다.
        InitializeMinimap(FoundManager);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MinimapWidget: 월드에서 ADungeonManager를 찾을 수 없습니다!"));
    }
}

void UDungeonMinimapWidget::InitializeMinimap(ADungeonManager* InDungeonManager)
{
    if (InDungeonManager)
    {
        DungeonManager = InDungeonManager;
        
        // 던전 매니저의 델리게이트에 내 갱신 함수를 연결!
        DungeonManager->OnPlayerMovedRoom.AddDynamic(this, &UDungeonMinimapWidget::RefreshMapBoard);

        // 최초 1회 그리기
        RefreshMapBoard();
    }
}

void UDungeonMinimapWidget::RefreshMapBoard()
{
    if (!DungeonManager || !MapBoard || !RoomIconClass) return;

    MapBoard->ClearChildren();

    // 1. 플레이어의 현재 논리 좌표 가져오기
    FIntPoint PlayerCoord = DungeonManager->GetCurrentPlayerCoordinate();

    // 2. 현재 좌표 기준 -2 ~ +2 범위만 루프 (5x5 영역)
    for (int32 X = PlayerCoord.X - 2; X <= PlayerCoord.X + 2; ++X)
    {
        for (int32 Y = PlayerCoord.Y - 2; Y <= PlayerCoord.Y + 2; ++Y)
        {
            FIntPoint TargetCoord(X, Y);
            
            // DungeonManager의 데이터 맵에서 해당 좌표의 방 정보 확인
            const TMap<FIntPoint, FRoomStatus>& RoomData = DungeonManager->GetRoomDataMap();
            if (RoomData.Contains(TargetCoord))
            {
                const FRoomStatus& Status = RoomData[TargetCoord];

                // 3. 조건: 존재를 아는 방만 그리기
                if (Status.VisitStatus != ERoomVisitStatus::Unknown)
                {
                    UMinimapRoomIcon* NewIcon = CreateWidget<UMinimapRoomIcon>(this, RoomIconClass);
                    
                    if (NewIcon && Status.RoomClass)
                    {
                        if (ARoomBase* CDO = Cast<ARoomBase>(Status.RoomClass->GetDefaultObject()))
                        {
                            if (Status.VisitStatus == ERoomVisitStatus::Unvisited)
                            {
                                NewIcon->SetupIcon(CommonBorderTexture, UnvisitedIconTexture, RoomIconSize);
                                NewIcon->CenterIconImage->SetOpacity(0.0f); // 방문하지 않은 방은 중앙 아이콘을 투명하게 처리
                            }
                            else
                            {
                                NewIcon->SetupIcon(CommonBorderTexture, CDO->GetIcon(), RoomIconSize);
                            }
                            
                        }

                        UCanvasPanelSlot* CanvasSlot = MapBoard->AddChildToCanvas(NewIcon);
                        
                        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
                        
                        // 주석 처리하셨던 Alignment는 주석을 푸셔야 합니다! (아이콘 중심을 0,0에 맞추기 위해 필수)
                        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f)); 
                        
                        // [수정된 부분] 플레이어와의 상대적인 위치(Offset)를 계산합니다.
                        int32 OffsetX = TargetCoord.X - PlayerCoord.X;
                        int32 OffsetY = TargetCoord.Y - PlayerCoord.Y;

                        // 상대 좌표를 바탕으로 UI 위치를 결정합니다.
                        // 플레이어 방(Offset 0,0)은 완벽하게 UILoc(0,0)에 그려집니다.
                        float UILocX = OffsetY * RoomIconSize;
                        float UILocY = OffsetX * -RoomIconSize;
                        
                        UE_LOG(LogTemp, Warning, TEXT("UILocX: %f, UILocY: %f"), UILocX, UILocY);

                        CanvasSlot->SetPosition(FVector2D(UILocX, UILocY));
                        
                        
                        
                        CanvasSlot->SetAutoSize(false);
                        CanvasSlot->SetSize(FVector2D(RoomIconSize, RoomIconSize));
                    }
                }
            }
        }
    }
}

// 매 프레임마다 플레이어의 위치와 시야에 맞춰 미니맵을 움직이고 돌립니다.
void UDungeonMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!DungeonManager || !MapRotationLayer || !MapBoard) return;
    

    // ====================================================================
    // 2. 회전: 플레이어의 시야에 맞춰 빙글빙글 돌리는 것은 그대로 유지합니다.
    // ====================================================================
    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerChar)
    {
        float PlayerYaw = PlayerChar->GetControlRotation().Yaw;
        MapRotationLayer->SetRenderTransformAngle(-PlayerYaw);
    }
}