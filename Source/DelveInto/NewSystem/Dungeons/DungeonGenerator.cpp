#include "DungeonGenerator.h"
#include "NewSystem/Dungeons/Props/Rooms/RoomBase.h"
#include "NewSystem/DelveDoor.h"

ADungeonGenerator::ADungeonGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ADungeonGenerator::BeginPlay()
{
    Super::BeginPlay();
    
    // [안전장치] 필수 클래스가 설정되었는지 확인
    if (RoomPresets.Num() == 0 || !DoorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("DungeonGenerator: RoomPreset or DoorClass is NOT selected in Blueprint!"));
        return;
    }

    CreateLayout();
    SpawnRooms();
    SpawnDoorsAndLink();
}

// 1. 방 배치 알고리즘 (Random Walk + Dead End Attachment)
void ADungeonGenerator::CreateLayout()
{
    bool bValidLayout = false;
    int32 LayoutAttempts = 0; // 무한 루프 방지용

    // 보스 방이 무사히 배치될 때까지 레이아웃 생성을 반복합니다. (최대 100번)
    while (!bValidLayout && LayoutAttempts < 100)
    {
        LayoutAttempts++;
        RoomDataMap.Empty();

        // 시작 방 (0,0)
        FIntPoint CurrentCoord(0, 0);
        RoomDataMap.Add(CurrentCoord, FRoomData(CurrentCoord, ERoomType::Start));

        int32 SpecialRoomCount = 2; // Boss, NPC
        int32 TargetNormalRooms = FMath::Max(1, MaxRoomCount - SpecialRoomCount);

        int32 MaxIterations = TargetNormalRooms * 10;
        int32 CurrentIterations = 0;

        // 1단계: 목표 개수만큼 일반 방(Normal)을 랜덤 워크로 생성
        while (RoomDataMap.Num() < TargetNormalRooms && CurrentIterations < MaxIterations)
        {
            CurrentIterations++;

            int32 RandDir = FMath::RandRange(0, 3);
            FIntPoint NextCoord = CurrentCoord;

            switch (RandDir)
            {
                case 0: NextCoord.X += 1; break; // Forward
                case 1: NextCoord.Y += 1; break; // Right
                case 2: NextCoord.X -= 1; break; // Backward
                case 3: NextCoord.Y -= 1; break; // Left
            }

            if (!RoomDataMap.Contains(NextCoord))
            {
                RoomDataMap.Add(NextCoord, FRoomData(NextCoord, ERoomType::Normal));
            }
            CurrentCoord = NextCoord; 
        }

        // 2단계: [필수] 보스 방 부착 시도
        // 막다른 길이 없어서 실패하면 false가 반환되며, 루프가 처음부터 다시 돕니다!
        bValidLayout = AttachSpecialRoom(ERoomType::Boss, true);

        if (bValidLayout)
        {
            // 3단계: [선택] NPC 방 부착 시도 (기본값 false를 사용하여 랜덤 막다른 길에 배치)
            AttachSpecialRoom(ERoomType::NPC);
        }
    }

    if (!bValidLayout)
    {
        UE_LOG(LogTemp, Error, TEXT("DungeonGenerator: 100번 시도했지만 보스 방을 배치할 수 없었습니다."));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("DungeonGenerator: 던전 레이아웃 생성 성공!"));
    }
}

// [수정] 성공 시 true, 실패 시 false 반환
// [수정] 가장 먼 거리를 계산하는 로직 추가
bool ADungeonGenerator::AttachSpecialRoom(ERoomType RoomType, bool bUseFurthest)
{
    TArray<FIntPoint> PotentialSpots;

    // 1. 주변에 빈자리가 있고, 그 빈자리의 이웃이 딱 1개(막다른 길)인 곳 탐색
    for (const auto& Elem : RoomDataMap)
    {
        FIntPoint Coord = Elem.Key;
        
        FIntPoint Neighbors[4] = {
            FIntPoint(Coord.X + 1, Coord.Y),
            FIntPoint(Coord.X - 1, Coord.Y),
            FIntPoint(Coord.X, Coord.Y + 1),
            FIntPoint(Coord.X, Coord.Y - 1)
        };

        for (FIntPoint NeighborCoord : Neighbors)
        {
            if (!RoomDataMap.Contains(NeighborCoord) && GetNeighborCount(NeighborCoord) == 1)
            {
                PotentialSpots.AddUnique(NeighborCoord);
            }
        }
    }

    // 2. 후보지 중 하나를 선택하여 배치
    if (PotentialSpots.Num() > 0)
    {
        int32 SelectedIndex = 0;

        if (bUseFurthest)
        {
            // [핵심] (0,0)과의 거리가 가장 먼 좌표를 찾습니다.
            float MaxDistanceSq = -1.0f; // 최대 거리 저장용
            
            for (int32 i = 0; i < PotentialSpots.Num(); ++i)
            {
                FIntPoint Spot = PotentialSpots[i];
                
                // 유클리드 거리의 제곱 (X^2 + Y^2) 계산. 시작점이 0,0이므로 이렇게만 계산해도 됩니다.
                float DistSq = FMath::Square((float)Spot.X) + FMath::Square((float)Spot.Y);
                
                if (DistSq > MaxDistanceSq)
                {
                    MaxDistanceSq = DistSq;
                    SelectedIndex = i; // 가장 먼 곳의 인덱스로 갱신
                }
            }
        }
        else
        {
            // 일반적인 경우(NPC 방 등)에는 랜덤으로 선택
            SelectedIndex = FMath::RandRange(0, PotentialSpots.Num() - 1);
        }

        // 최종 선택된 좌표에 방 배치
        RoomDataMap.Add(PotentialSpots[SelectedIndex], FRoomData(PotentialSpots[SelectedIndex], RoomType));
        return true; 
    }
    
    // 배치할 수 있는 빈 자리가 없음
    return false; 
}

// [신규] 특정 좌표 주변 4방향에 존재하는 방의 개수 반환
int32 ADungeonGenerator::GetNeighborCount(FIntPoint Coord)
{
    int32 Count = 0;
    if (RoomDataMap.Contains(FIntPoint(Coord.X + 1, Coord.Y))) Count++;
    if (RoomDataMap.Contains(FIntPoint(Coord.X - 1, Coord.Y))) Count++;
    if (RoomDataMap.Contains(FIntPoint(Coord.X, Coord.Y + 1))) Count++;
    if (RoomDataMap.Contains(FIntPoint(Coord.X, Coord.Y - 1))) Count++;
    return Count;
}

// 2. 실제 방 액터 스폰
void ADungeonGenerator::SpawnRooms()
{
    SpawnedRoomMap.Empty();
    
    // 안전장치: 프리셋이 아예 비어있으면 중단
    if (RoomPresets.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("DungeonGenerator: No Room Presets assigned!"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (auto& Elem : RoomDataMap)
    {
        FIntPoint Coord = Elem.Key;
        ERoomType Type = Elem.Value.Type; // 현재 좌표의 방 타입 (Start, Normal, Boss...)
        
        // 1. [핵심] 타입에 맞는 랜덤 방 클래스 가져오기
        TSubclassOf<ARoomBase> SelectedClass = GetRandomRoomClass(Type);

        if (!SelectedClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("No class found for RoomType: %d"), (int32)Type);
            continue;
        }

        // 2. 스폰
        FVector SpawnLocation(Coord.X * RoomGridSize, Coord.Y * RoomGridSize, 0.0f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        ARoomBase* NewRoom = GetWorld()->SpawnActor<ARoomBase>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);
        
        if (NewRoom)
        {
            SpawnedRoomMap.Add(Coord, NewRoom);
            
            // 필요하다면 여기서 방 타입 정보를 방에게도 알려줌
            // NewRoom->Type = Type; 
        }
    }
}

// 3. 문 스폰 및 포인터 연결 & 벽 허물기 (핵심 로직)
void ADungeonGenerator::SpawnDoorsAndLink()
{
    if (!DoorClass) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // "Forward"와 "Right" 방향만 검사하여 중복 생성 방지
    // (내 Forward는 앞집의 Backward와 같고, 내 Right는 옆집의 Left와 같음)
    
    for (auto& Elem : SpawnedRoomMap)
    {
        FIntPoint MyCoord = Elem.Key;
        ARoomBase* MyRoom = Elem.Value;

        // 검사할 방향 목록 (상, 우)
        ERoomDirection CheckDirs[] = { ERoomDirection::Forward, ERoomDirection::Right };

        for (ERoomDirection Dir : CheckDirs)
        {
            // 1. 이웃 좌표 구하기
            FIntPoint NeighborCoord = GetNeighborCoordinate(MyCoord, Dir);

            // 2. 이웃 방이 존재하는지 확인
            if (SpawnedRoomMap.Contains(NeighborCoord))
            {
                ARoomBase* NeighborRoom = SpawnedRoomMap[NeighborCoord];

                // 3. 문 스폰 위치 계산 (두 방의 중간 지점)
                FVector MyLoc = MyRoom->GetActorLocation();
                FVector NeighborLoc = NeighborRoom->GetActorLocation();
                FVector DoorLoc = (MyLoc + NeighborLoc) * 0.5f;

                // 4. 문 회전 계산
                // Forward(X축 연결) -> 문 회전 0 (또는 90, 메쉬에 따라 다름)
                // Right(Y축 연결) -> 문 회전 90
                FRotator DoorRot = FRotator::ZeroRotator;
                if (Dir == ERoomDirection::Right) 
                {
                    DoorRot.Yaw = 90.0f; 
                }

                // 5. 문 스폰
                ADelveDoor* NewDoor = GetWorld()->SpawnActor<ADelveDoor>(DoorClass, DoorLoc, DoorRot, SpawnParams);

                if (NewDoor)
                {
                    // 6. [연결] 양쪽 방에 문 포인터 등록
                    
                    // A. 내 방(MyRoom) 처리
                    MyRoom->Doors.Add(Dir, NewDoor);     // 문 등록
                    MyRoom->OpenWall(Dir);               // [핵심] 해당 방향 벽 허물기

                    // B. 이웃 방(NeighborRoom) 처리
                    ERoomDirection OpDir = GetOppositeDirection(Dir); // 반대 방향 (예: Backward)
                    NeighborRoom->Doors.Add(OpDir, NewDoor); // 문 등록
                    NeighborRoom->OpenWall(OpDir);           // [핵심] 반대 방향 벽 허물기
                    
                    // 디버그 로그 (필요시 주석 해제)
                    // UE_LOG(LogTemp, Log, TEXT("Linked Room[%d,%d]-(%s) <-> Room[%d,%d]-(%s)"), 
                    //     MyCoord.X, MyCoord.Y, *UEnum::GetValueAsString(Dir),
                    //     NeighborCoord.X, NeighborCoord.Y, *UEnum::GetValueAsString(OpDir));
                }
            }
        }
    }
}

// 헬퍼: 좌표 계산
FIntPoint ADungeonGenerator::GetNeighborCoordinate(FIntPoint Current, ERoomDirection Direction)
{
    switch (Direction)
    {
        case ERoomDirection::Forward:  return FIntPoint(Current.X + 1, Current.Y);
        case ERoomDirection::Right:    return FIntPoint(Current.X, Current.Y + 1);
        case ERoomDirection::Backward: return FIntPoint(Current.X - 1, Current.Y);
        case ERoomDirection::Left:     return FIntPoint(Current.X, Current.Y - 1);
    }
    return Current;
}

// 헬퍼: 반대 방향
ERoomDirection ADungeonGenerator::GetOppositeDirection(ERoomDirection Direction)
{
    switch (Direction)
    {
        case ERoomDirection::Forward:  return ERoomDirection::Backward;
        case ERoomDirection::Right:    return ERoomDirection::Left;
        case ERoomDirection::Backward: return ERoomDirection::Forward;
        case ERoomDirection::Left:     return ERoomDirection::Right;
    }
    return ERoomDirection::Forward;
}

TSubclassOf<ARoomBase> ADungeonGenerator::GetRandomRoomClass(ERoomType Type)
{
    // 1. 해당 타입(Start, Normal 등)의 프리셋이 있는지 확인
    if (RoomPresets.Contains(Type))
    {
        const TArray<TSubclassOf<ARoomBase>>& Candidates = RoomPresets[Type].Variations;

        // 2. 후보가 하나라도 있다면 랜덤 선택
        if (Candidates.Num() > 0)
        {
            int32 RandIndex = FMath::RandRange(0, Candidates.Num() - 1);
            return Candidates[RandIndex];
        }
    }

    // 프리셋이 비어있으면 Normal 타입에서라도 찾아봄 (Fallback)
    if (Type != ERoomType::Normal && RoomPresets.Contains(ERoomType::Normal))
    {
        return GetRandomRoomClass(ERoomType::Normal);
    }

    return nullptr;
}