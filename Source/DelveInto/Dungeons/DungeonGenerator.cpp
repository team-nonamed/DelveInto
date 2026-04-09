#include "DungeonGenerator.h"

#include "Algo/RandomShuffle.h"
#include "Chaos/EPA.h"
#include "NewSystem/DelveDoor.h"

DEFINE_LOG_CATEGORY(LogRoomPreset);

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

void ADungeonGenerator::CreateLayout()
{
    bool bValidLayout = false;
    int32 LayoutAttempts = 0;

    while (!bValidLayout && LayoutAttempts < 100)
    {
        LayoutAttempts++;
        RoomDataMap.Empty();

        // ================================================================================
        // 1. 개수 및 배열 준비 (수정됨: 0으로 나누기 및 float 형변환 방지)
        // ================================================================================
        int32 RoomTotalCount = FMath::RandRange(RoomCountLimit.MinCount, RoomCountLimit.MaxCount);
        int32 RoomCurrentCount = 0;

        TMap<ERoomType, int32> TypedRoomTotalCounts;
        TMap<ERoomType, int32> TypedRoomCurrentCounts;
        TypedRoomTotalCounts[ERoomType::Start] = 1; 
        
        TSet<ERoomType> PlainRoomTypes;
        TArray<ERoomType> MainRoomSpawnOrder;
        int32 PlainRoomCount = RoomTotalCount - 1;
        
        for (auto& Pair: RoomPresets)
        {
            ERoomType Type = Pair.Key;
            FRoomTypeConfig& Config = Pair.Value;

            if (Type == ERoomType::Start) continue;

            if (!Config.bIsConnectedByOneConnector)
            {
                if (!Config.bHasLimitCount)
                {
                    PlainRoomTypes.Add(Type);
                    continue;
                }

                int32 Count = FMath::RandRange(Config.LimitCounts.MinCount, Config.LimitCounts.MaxCount);
                TypedRoomTotalCounts.Add(Type, Count);

                for (int i = 0; i < Count; i++)
                {
                    MainRoomSpawnOrder.Add(Type);
                    PlainRoomCount--;
                    if (PlainRoomCount <= 0) break;
                }
                if (PlainRoomCount <= 0) break;
            }
        }

        if (PlainRoomCount > 0 && PlainRoomTypes.Num() > 0)
        {
            int32 RoomTypeMaxNumber = 0;
            for (auto Type: PlainRoomTypes) RoomTypeMaxNumber += RoomPresets[Type].SpawnWeight;

            if (RoomTypeMaxNumber > 0) // 0으로 나누기 방지
            {
                int32 RemainPlainRoomCount = PlainRoomCount;
                for (auto Type: PlainRoomTypes)
                {
                    // (float) 캐스팅을 통해 정확한 비율 계산
                    int32 Count = FMath::Min(FMath::CeilToInt32((float)RoomPresets[Type].SpawnWeight * PlainRoomCount / RoomTypeMaxNumber), RemainPlainRoomCount);
                    
                    TypedRoomTotalCounts.Add(Type, Count);
                    for (int i = 0; i < Count; i++) MainRoomSpawnOrder.Add(Type);
                    RemainPlainRoomCount -= Count;
                }
            }
        }
        Algo::RandomShuffle(MainRoomSpawnOrder);

        // ================================================================================
        // 2. 중심에 시작방 배치
        // ================================================================================
        FIntPoint CurrentCoord(0, 0);
        RoomDataMap.Add(CurrentCoord, FRoomStatus(CurrentCoord, ERoomType::Start, GetRandomRoomClass(ERoomType::Start)));
        TypedRoomCurrentCounts.Add(ERoomType::Start, 1);

        TArray<FIntPoint> Directions = { FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1) };

        // ================================================================================
        // 3. 주 뼈대 방을 생성 (수정됨: BFS 큐 로직 완벽 적용)
        // ================================================================================
        TQueue<FIntPoint> TraversingQueue;
        TraversingQueue.Enqueue(CurrentCoord);

        // MainRoomSpawnOrder에 남은 방이 있는 동안 루프
        while (MainRoomSpawnOrder.Num() > 0)
        {
            FIntPoint ParentCoord;
            
            // 큐가 비었는데 남은 방이 있다면(사방이 꽉 막힘), 레이아웃 실패 처리 후 다시 시도
            if (!TraversingQueue.Dequeue(ParentCoord)) 
            {
                break; // while을 빠져나가면 배열이 남아있으므로 Layout 실패 처리됨
            }

            int32 PossibleSpawnable = 4 - GetNeighborCount(ParentCoord);
            if (PossibleSpawnable <= 0) continue;

            // 부모 방에서 몇 개의 가지를 칠지 결정
            int32 Branches = FMath::RandRange(1, PossibleSpawnable);
            int32 SpawnedThisTurn = 0;

            Algo::RandomShuffle(Directions);

            for (const FIntPoint& Dir: Directions)
            {
                if (SpawnedThisTurn >= Branches) break;          // 이번 턴의 할당량 채움
                if (MainRoomSpawnOrder.Num() == 0) break;       // 더 이상 스폰할 방이 없음

                FIntPoint NeighborCoord = ParentCoord + Dir;
                if (RoomDataMap.Contains(NeighborCoord)) continue;

                // [핵심] 배열의 끝에서 방 타입을 하나 꺼냄 (Pop)
                ERoomType NextType = MainRoomSpawnOrder.Pop(false);

                // 방 배치
                RoomDataMap.Add(NeighborCoord, FRoomStatus(NeighborCoord, NextType, GetRandomRoomClass(NextType)));
                
                int32& CurrentTypeCount = TypedRoomCurrentCounts.FindOrAdd(NextType);
                CurrentTypeCount++;

                TraversingQueue.Enqueue(NeighborCoord);
                SpawnedThisTurn++;
            }
        }

        // 큐가 막혀서 배열을 다 못 비웠다면 다시 생성 (bValidLayout은 여전히 false)
        if (MainRoomSpawnOrder.Num() > 0) continue;

        // ================================================================================
        // 4. 특수 방들을 생성 (수정됨: MinCount 반복 적용)
        // ================================================================================
        bValidLayout = true;
        for (auto& Pair : RoomPresets)
        {
            ERoomType CurrentType = Pair.Key;
            FRoomTypeConfig& Config = Pair.Value;

            if (CurrentType == ERoomType::Normal || CurrentType == ERoomType::Start) continue;
            if (!Config.bIsConnectedByOneConnector) continue; // 이미 뼈대에서 만든 방은 제외

            int32 SpawnCount = Config.bHasLimitCount ? Config.LimitCounts.MinCount : 1;
            
            for (int i = 0; i < SpawnCount; i++)
            {
                if (!AttachSpecialRoom(CurrentType))
                {
                    if (CurrentType == ERoomType::Boss) bValidLayout = false;
                }
            }
        }
    }
}

bool ADungeonGenerator::AttachSpecialRoom(ERoomType RoomType)
{
    // 1. 해당 타입의 설정 가져오기
    if (!RoomPresets.Contains(RoomType)) return false;
    const FRoomTypeConfig& Config = RoomPresets[RoomType];

    // [안전장치] 만약 이 함수가 여러 번 불려서 MaxCount를 넘기려 한다면 차단
    if (Config.bHasLimitCount)
    {
        int32 CurrentCount = 0;
        for (const auto& Elem : RoomDataMap) { if (Elem.Value.Type == RoomType) CurrentCount++; }
        if (CurrentCount >= Config.LimitCounts.MaxCount) return false;
    }

    TArray<FIntPoint> PotentialSpots;
    
    // 2. 후보지 검색 (막다른 길 찾기)
    for (const auto& Elem : RoomDataMap)
    {
        if (Elem.Value.Type != ERoomType::Normal && Elem.Value.Type != ERoomType::Start) continue;

        FIntPoint Coord = Elem.Key;
        FIntPoint Neighbors[4] = {
            FIntPoint(Coord.X + 1, Coord.Y), FIntPoint(Coord.X - 1, Coord.Y),
            FIntPoint(Coord.X, Coord.Y + 1), FIntPoint(Coord.X, Coord.Y - 1)
        };

        for (FIntPoint NeighborCoord : Neighbors)
        {
            if (!RoomDataMap.Contains(NeighborCoord) && GetNeighborCount(NeighborCoord) == 1)
            {
                PotentialSpots.AddUnique(NeighborCoord);
            }
        }
    }

    // 3. 후보지 중 선택
    if (PotentialSpots.Num() > 0)
    {
        int32 SelectedIndex = 0;
        
        if (Config.bFurthestSpawn)
        {
            int32 MaxDistance = -1;
            TArray<int32> MaxDistanceIndices; // 최대 거리가 동일한 후보들을 담을 배열

            for (int32 i = 0; i < PotentialSpots.Num(); ++i)
            {
                // [개선] 맨해튼 거리(Manhattan Distance) 사용: 그리드 맵에서의 실제 체감 거리
                int32 GridDist = FMath::Abs(PotentialSpots[i].X) + FMath::Abs(PotentialSpots[i].Y);
                
                if (GridDist > MaxDistance)
                {
                    MaxDistance = GridDist;
                    MaxDistanceIndices.Empty(); // 더 먼 곳을 찾았으니 기존 목록 초기화
                    MaxDistanceIndices.Add(i);
                }
                else if (GridDist == MaxDistance)
                {
                    MaxDistanceIndices.Add(i); // 거리가 같다면 후보군에 추가
                }
            }

            // [개선] 최고 거리가 여러 개라면 그 중 랜덤 선택 (편향 방지)
            int32 RandomChoice = FMath::RandRange(0, MaxDistanceIndices.Num() - 1);
            SelectedIndex = MaxDistanceIndices[RandomChoice];
        }
        else
        {
            SelectedIndex = FMath::RandRange(0, PotentialSpots.Num() - 1);
        }

        FIntPoint FinalCoord = PotentialSpots[SelectedIndex];
        TSubclassOf<ARoomBase> SelectedClass = GetRandomRoomClass(RoomType);
        
        RoomDataMap.Add(FinalCoord, FRoomStatus(FinalCoord, RoomType, SelectedClass));
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
        
        // [수정 1] Layout 단계에서 미리 결정해 둔 방 클래스를 그대로 사용합니다.
        TSubclassOf<ARoomBase> SelectedClass = Elem.Value.RoomClass;

        if (!SelectedClass) continue;

        FVector SpawnLocation(Coord.X * RoomGridSize, Coord.Y * RoomGridSize, 0.0f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        // [수정 2] 하드코딩(Boss, NPC)을 제거하고, Preset의 속성(bIsConnectedByOneConnector)으로 막다른 방인지 판단합니다.
        bool bIsDeadEndRoom = false;
        if (RoomPresets.Contains(Type) && Type != ERoomType::Start)
        {
            bIsDeadEndRoom = RoomPresets[Type].bIsConnectedByOneConnector;
        }

        if (bIsDeadEndRoom)
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
        if (NewRoom)
        {
            SpawnedRoomMap.Add(Coord, NewRoom);

            // [유지] 생성된 방의 플레이어 진입 이벤트를 구독합니다.
            NewRoom->OnPlayerEnteredRoom.AddDynamic(this, &ADungeonGenerator::HandleRoomExplored);
        }
    }
}

void ADungeonGenerator::HandleRoomExplored(ARoomBase* ExploredRoom)
{
    if (!ExploredRoom) return;

    // 1. 역으로 좌표 찾기 (SpawnedRoomMap에서 찾거나, 방 자체가 좌표를 들고 있게 수정 가능)
    const FIntPoint* FoundCoord = SpawnedRoomMap.FindKey(ExploredRoom);
    if (FoundCoord)
    {
        CurrentPlayerCoordinate = *FoundCoord;
        
        // 2. RoomDataMap의 상태 갱신
        if (RoomDataMap.Contains(CurrentPlayerCoordinate))
        {
            RoomDataMap[CurrentPlayerCoordinate].bIsPlayerVisited = true;
        }

        UE_LOG(LogTemp, Log, TEXT("DungeonGenerator: Player is now in Room %s"), *CurrentPlayerCoordinate.ToString());

        // 3. TODO: 여기서 UI(미니맵)에 갱신 명령을 내립니다.
    }
}

void ADungeonGenerator::SpawnDoorsAndLink()
{
    // 기존의 if (!DoorClass) return; 삭제 -> 방의 Spawner가 직접 결정하므로 불필요

    for (auto& Elem : SpawnedRoomMap)
    {
        FIntPoint MyCoord = Elem.Key;
        ARoomBase* MyRoom = Elem.Value;

        ERoomType MyType = RoomDataMap[MyCoord].Type;
        bool bIsMyRoomSpecial = false;
        if (RoomPresets.Contains(MyType) && MyType != ERoomType::Start)
        {
            bIsMyRoomSpecial = RoomPresets[MyType].bIsConnectedByOneConnector;
        }

        ESotaDirection CheckDirs[] = { ESotaDirection::Forward, ESotaDirection::Right };

        for (ESotaDirection Dir : CheckDirs)
        {
            FIntPoint NeighborCoord = GetNeighborCoordinate(MyCoord, Dir);
            if (!SpawnedRoomMap.Contains(NeighborCoord)) continue;

            ARoomBase* NeighborRoom = SpawnedRoomMap[NeighborCoord];
            ERoomType NeighborType = RoomDataMap[NeighborCoord].Type;

            bool bNeighborIsSpecial = false;
            if (RoomPresets.Contains(NeighborType) && NeighborType != ERoomType::Start)
            {
                bNeighborIsSpecial = RoomPresets[NeighborType].bIsConnectedByOneConnector;
            }

            // [특수방 로직 유지]
            if (bIsMyRoomSpecial)
            {
                FVector WorldDirVec = FVector(NeighborCoord.X - MyCoord.X, NeighborCoord.Y - MyCoord.Y, 0.0f);
                FVector LocalForwardVec = MyRoom->GetActorForwardVector();
                if (!WorldDirVec.GetSafeNormal().Equals(LocalForwardVec, 0.1f)) continue;
            }

            if (bNeighborIsSpecial)
            {
                FVector WorldToMeVec = FVector(MyCoord.X - NeighborCoord.X, MyCoord.Y - NeighborCoord.Y, 0.0f);
                FVector NeighborForwardVec = NeighborRoom->GetActorForwardVector();
                if (!WorldToMeVec.GetSafeNormal().Equals(NeighborForwardVec, 0.1f)) continue;
            }

            // =============================================================
            // [변경됨] ConnectorSpawner를 통한 스폰 및 연결 로직
            // =============================================================
            
            // 1. MyRoom을 주체로 삼아, 해당 방향의 Spawner 설정대로 Connector 스폰
            ARoomConnector* SpawnedConnector = MyRoom->SpawnConnector(Dir);

            if (SpawnedConnector)
            {
                // 2. 스폰 성공 시, 이웃 방에게 이 커넥터를 공유하고 등록시킴
                ESotaDirection OpDir = GetOppositeDirection(Dir);
                NeighborRoom->RegisterConnector(OpDir, SpawnedConnector);
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