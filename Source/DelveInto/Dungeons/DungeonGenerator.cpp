#include "DungeonGenerator.h"
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
        RoomDataMap.Add(CurrentCoord, FRoomStatus(CurrentCoord, ERoomType::Start));

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
        // =============================================================
        // [핵심 수정] 보스 방이나 NPC 방을 '징검다리'로 삼지 않도록 차단!
        // 오직 일반 방(Normal)이나 시작 방(Start) 옆에만 특수 방이 붙을 수 있습니다.
        // =============================================================
        if (Elem.Value.Type == ERoomType::Boss || Elem.Value.Type == ERoomType::NPC)
        {
            continue;
        }

        FIntPoint Coord = Elem.Key;
        
        FIntPoint Neighbors[4] = {
            FIntPoint(Coord.X + 1, Coord.Y),
            FIntPoint(Coord.X - 1, Coord.Y),
            FIntPoint(Coord.X, Coord.Y + 1),
            FIntPoint(Coord.X, Coord.Y - 1)
        };

        for (FIntPoint NeighborCoord : Neighbors)
        {
            // 이 빈 자리(NeighborCoord)의 이웃이 딱 1개라는 뜻은,
            // 방금 걸러내고 남은 '일반 방'만이 유일한 이웃이라는 뜻이 보장됩니다.
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
            float MaxDistanceSq = -1.0f; 
            for (int32 i = 0; i < PotentialSpots.Num(); ++i)
            {
                FIntPoint Spot = PotentialSpots[i];
                float DistSq = FMath::Square((float)Spot.X) + FMath::Square((float)Spot.Y);
                
                if (DistSq > MaxDistanceSq)
                {
                    MaxDistanceSq = DistSq;
                    SelectedIndex = i; 
                }
            }
        }
        else
        {
            SelectedIndex = FMath::RandRange(0, PotentialSpots.Num() - 1);
        }

        RoomDataMap.Add(PotentialSpots[SelectedIndex], FRoomData(PotentialSpots[SelectedIndex], RoomType));
        return true; 
    }
    
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

void ADungeonGenerator::SpawnRooms()
{
    SpawnedRoomMap.Empty();
    if (RoomPresets.Num() == 0) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (auto& Elem : RoomDataMap)
    {
        FIntPoint Coord = Elem.Key;
        ERoomType Type = Elem.Value.Type;
        TSubclassOf<ARoomBase> SelectedClass = GetRandomRoomClass(Type);

        if (!SelectedClass) continue;

        FVector SpawnLocation(Coord.X * RoomGridSize, Coord.Y * RoomGridSize, 0.0f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        // =============================================================
        // [신규] 특수 방(Boss, NPC) 회전 계산 로직
        // =============================================================
        if (Type == ERoomType::Boss || Type == ERoomType::NPC)
        {
            // 이 방과 연결된 유일한 이웃(부모) 찾기
            FIntPoint Neighbors[4] = {
                FIntPoint(Coord.X + 1, Coord.Y), FIntPoint(Coord.X - 1, Coord.Y),
                FIntPoint(Coord.X, Coord.Y + 1), FIntPoint(Coord.X, Coord.Y - 1)
            };

            for (int32 i = 0; i < 4; i++)
            {
                if (RoomDataMap.Contains(Neighbors[i]))
                {
                    // 부모 방이 내 기준으로 어느 방향에 있는지 계산
                    FIntPoint Diff = Neighbors[i] - Coord;
                    
                    if (Diff.X == 1)  SpawnRotation.Yaw = 0.0f;   // 부모가 앞(+X)에 있음
                    if (Diff.X == -1) SpawnRotation.Yaw = 180.0f; // 부모가 뒤(-X)에 있음
                    if (Diff.Y == 1)  SpawnRotation.Yaw = 90.0f;  // 부모가 우(+Y)에 있음
                    if (Diff.Y == -1) SpawnRotation.Yaw = -90.0f; // 부모가 좌(-Y)에 있음
                    break; 
                }
            }
        }

        ARoomBase* NewRoom = GetWorld()->SpawnActor<ARoomBase>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (NewRoom) SpawnedRoomMap.Add(Coord, NewRoom);
    }
}

void ADungeonGenerator::SpawnDoorsAndLink()
{
    if (!DoorClass) return;

    for (auto& Elem : SpawnedRoomMap)
    {
        FIntPoint MyCoord = Elem.Key;
        ARoomBase* MyRoom = Elem.Value;

        // 특수 방인지 확인 (RoomDataMap에서 타입 체크 필요하므로 아래처럼 접근)
        ERoomType MyType = RoomDataMap[MyCoord].Type;
        bool bIsSpecial = (MyType == ERoomType::Boss || MyType == ERoomType::NPC);

        ESotaDirection CheckDirs[] = { ESotaDirection::Forward, ESotaDirection::Right };

        for (ESotaDirection Dir : CheckDirs)
        {
            FIntPoint NeighborCoord = GetNeighborCoordinate(MyCoord, Dir);
            if (!SpawnedRoomMap.Contains(NeighborCoord)) continue;

            ARoomBase* NeighborRoom = SpawnedRoomMap[NeighborCoord];
            ERoomType NeighborType = RoomDataMap[NeighborCoord].Type;

            // =============================================================
            // [핵심 로직] 특수 방 통로 제한
            // =============================================================
            
            // 1. 내가 특수 방인데, 현재 검사하는 방향이 내 로컬 Forward(+X)가 아니면 문 안 만듦
            if (bIsSpecial)
            {
                // 월드 좌표계의 Dir가 내 로컬 Forward인지 확인
                FVector WorldDirVec = FVector(NeighborCoord.X - MyCoord.X, NeighborCoord.Y - MyCoord.Y, 0.0f);
                FVector LocalForwardVec = MyRoom->GetActorForwardVector();
                
                if (!WorldDirVec.GetSafeNormal().Equals(LocalForwardVec, 0.1f)) continue;
            }

            // 2. 이웃이 특수 방인데, 그 방의 로컬 Forward가 나를 향하고 있지 않으면 문 안 만듦
            if (NeighborType == ERoomType::Boss || NeighborType == ERoomType::NPC)
            {
                FVector WorldToMeVec = FVector(MyCoord.X - NeighborCoord.X, MyCoord.Y - NeighborCoord.Y, 0.0f);
                FVector NeighborForwardVec = NeighborRoom->GetActorForwardVector();

                if (!WorldToMeVec.GetSafeNormal().Equals(NeighborForwardVec, 0.1f)) continue;
            }

            // --- 이하 문 스폰 및 연결 로직 동일 ---
            FVector DoorLoc = (MyRoom->GetActorLocation() + NeighborRoom->GetActorLocation()) * 0.5f;
            FRotator DoorRot = (Dir == ESotaDirection::Right) ? FRotator(0, 90, 0) : FRotator::ZeroRotator;

            ADelveDoor* NewDoor = GetWorld()->SpawnActor<ADelveDoor>(DoorClass, DoorLoc, DoorRot);
            if (NewDoor)
            {
                // MyRoom 입장
                MyRoom->Doors.Add(Dir, NewDoor);
                MyRoom->OpenWall(Dir);

                // NeighborRoom 입장
                ESotaDirection OpDir = GetOppositeDirection(Dir);
                NeighborRoom->Doors.Add(OpDir, NewDoor);
                NeighborRoom->OpenWall(OpDir);
            }
        }
    }
}

// 헬퍼: 좌표 계산
FIntPoint ADungeonGenerator::GetNeighborCoordinate(FIntPoint Current, ESotaDirection Direction)
{
    switch (Direction)
    {
        case ESotaDirection::Forward:  return FIntPoint(Current.X + 1, Current.Y);
        case ESotaDirection::Right:    return FIntPoint(Current.X, Current.Y + 1);
        case ESotaDirection::Backward: return FIntPoint(Current.X - 1, Current.Y);
        case ESotaDirection::Left:     return FIntPoint(Current.X, Current.Y - 1);
    }
    return Current;
}

// 헬퍼: 반대 방향
ESotaDirection ADungeonGenerator::GetOppositeDirection(ESotaDirection Direction)
{
    switch (Direction)
    {
        case ESotaDirection::Forward:  return ESotaDirection::Backward;
        case ESotaDirection::Right:    return ESotaDirection::Left;
        case ESotaDirection::Backward: return ESotaDirection::Forward;
        case ESotaDirection::Left:     return ESotaDirection::Right;
    }
    return ESotaDirection::Forward;
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