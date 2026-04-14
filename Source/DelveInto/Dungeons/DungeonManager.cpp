#include "DungeonManager.h"
#include "Algo/RandomShuffle.h"


ADungeonManager::ADungeonManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ADungeonManager::BeginPlay()
{
    Super::BeginPlay();
    
    if (RoomPresets.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("DungeonGenerator: RoomPresets is empty!"));
        return;
    }

    CreateLayout();
    SpawnRooms();
    SpawnDoorsAndLink();
    
    CurrentPlayerCoordinate = FIntPoint(0, 0);
    if (RoomDataMap.Contains(CurrentPlayerCoordinate))
    {
        RoomDataMap[CurrentPlayerCoordinate].bIsPlayerVisited = true;
    }
}

// ==============================================================================
// 1. 레이아웃 생성 (가중치 + BFS + 특수방 맨해튼 거리 부착)
// ==============================================================================
void ADungeonManager::CreateLayout()
{
    bool bValidLayout = false;
    int32 LayoutAttempts = 0;

    while (!bValidLayout && LayoutAttempts < 100)
    {
        LayoutAttempts++;
        RoomDataMap.Empty();

        RoomCountLimit.Validate();
        int32 RoomTotalCount = FMath::RandRange(RoomCountLimit.MinCount, RoomCountLimit.MaxCount);
        
        TMap<ERoomType, int32> TypedRoomTotalCounts;
        TMap<ERoomType, int32> TypedRoomCurrentCounts;
        TypedRoomTotalCounts.Add(ERoomType::Start, 1);
        
        TSet<ERoomType> PlainRoomTypes;
        TArray<ERoomType> MainRoomSpawnOrder;
        int32 PlainRoomCount = RoomTotalCount - 1;
        
        // 프리셋 기반 필수 방(줄기) 계산
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

                Config.LimitCounts.Validate();
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

        // 가중치(Weight) 기반 일반 방 분배
        if (PlainRoomCount > 0 && PlainRoomTypes.Num() > 0)
        {
            int32 RoomTypeMaxNumber = 0;
            for (auto Type: PlainRoomTypes) RoomTypeMaxNumber += RoomPresets[Type].SpawnWeight;

            if (RoomTypeMaxNumber > 0)
            {
                int32 RemainPlainRoomCount = PlainRoomCount;
                for (auto Type: PlainRoomTypes)
                {
                    int32 Count = FMath::Min(FMath::CeilToInt32((float)RoomPresets[Type].SpawnWeight * PlainRoomCount / RoomTypeMaxNumber), RemainPlainRoomCount);
                    TypedRoomTotalCounts.Add(Type, Count);
                    for (int i = 0; i < Count; i++) MainRoomSpawnOrder.Add(Type);
                    RemainPlainRoomCount -= Count;
                }
            }
        }
        Algo::RandomShuffle(MainRoomSpawnOrder);

        // BFS 시작점 배치
        FIntPoint CurrentCoord(0, 0);
        RoomDataMap.Add(CurrentCoord, FRoomStatus(CurrentCoord, ERoomType::Start, GetRandomRoomClass(ERoomType::Start)));
        TypedRoomCurrentCounts.Add(ERoomType::Start, 1);

        TArray<FIntPoint> Directions = { FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1) };
        TQueue<FIntPoint> TraversingQueue;
        TraversingQueue.Enqueue(CurrentCoord);

        while (MainRoomSpawnOrder.Num() > 0)
        {
            FIntPoint ParentCoord;
            if (!TraversingQueue.Dequeue(ParentCoord)) break; // 막혀서 큐가 고갈됨

            // 1. 부모 방의 CDO를 가져와서 어느 방향이 막혀있는지 확인합니다.
            ARoomBase* ParentCDO = nullptr;
            if (RoomDataMap.Contains(ParentCoord) && RoomDataMap[ParentCoord].RoomClass)
            {
                ParentCDO = Cast<ARoomBase>(RoomDataMap[ParentCoord].RoomClass->GetDefaultObject());
            }

            // 2. 부모 방이 뻗어나갈 수 있는 진짜(Valid) 방향만 추려냅니다.
            TArray<FIntPoint> ValidDirections;
            for (const FIntPoint& Dir : Directions)
            {
                FIntPoint NeighborCoord = ParentCoord + Dir;
                if (RoomDataMap.Contains(NeighborCoord)) continue; // 이미 방이 있음
                
                ESotaDirection EnumDir = GetDirectionFromVector(Dir);
                if (ParentCDO && !ParentCDO->IsConnectorPlaceable(EnumDir)) continue; // 부모 방이 이 방향으론 막혀있음

                ValidDirections.Add(Dir);
            }

            if (ValidDirections.Num() == 0) continue; // 사방이 막혔거나 꽉 찼으면 스킵

            int32 Branches = FMath::RandRange(1, ValidDirections.Num());
            int32 SpawnedThisTurn = 0;

            Algo::RandomShuffle(ValidDirections);

            for (const FIntPoint& Dir : ValidDirections)
            {
                if (SpawnedThisTurn >= Branches) break;         
                if (MainRoomSpawnOrder.Num() == 0) break;       

                FIntPoint NeighborCoord = ParentCoord + Dir;
                ESotaDirection ParentEnumDir = GetDirectionFromVector(Dir);
                ESotaDirection RequiredOpDir = GetOppositeDirection(ParentEnumDir); // 자식 방은 이 방향이 반드시 뚫려있어야 함!

                // 3. 자식 방을 배치할 때, RequiredOpDir이 뚫려있는 클래스만 엄선해서 가져옵니다.
                ERoomType NextType = MainRoomSpawnOrder.Pop(EAllowShrinking::No);
                TSubclassOf<ARoomBase> CompatibleClass = GetCompatibleRoomClass(NextType, RequiredOpDir);

                RoomDataMap.Add(NeighborCoord, FRoomStatus(NeighborCoord, NextType, CompatibleClass));
                
                int32& CurrentTypeCount = TypedRoomCurrentCounts.FindOrAdd(NextType);
                CurrentTypeCount++;

                TraversingQueue.Enqueue(NeighborCoord);
                SpawnedThisTurn++;
            }
        }

        // 큐가 막혀서 배열을 다 못 비웠다면 재시도
        if (MainRoomSpawnOrder.Num() > 0) continue;

        // 특수 방(문 1개) 부착
        bValidLayout = true;
        for (auto& Pair : RoomPresets)
        {
            ERoomType CurrentType = Pair.Key;
            FRoomTypeConfig& Config = Pair.Value;

            if (CurrentType == ERoomType::Normal || CurrentType == ERoomType::Start) continue;
            if (!Config.bIsConnectedByOneConnector) continue; 

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

// ==============================================================================
// 특수방 부착 (막다른 길 탐색 및 맨해튼 거리 적용)
// ==============================================================================
bool ADungeonManager::AttachSpecialRoom(ERoomType RoomType)
{
    if (!RoomPresets.Contains(RoomType)) return false;
    const FRoomTypeConfig& Config = RoomPresets[RoomType];

    // 이제 PotentialSpots는 후보 좌표와, 맞물려야 할 방향을 함께 저장합니다.
    struct FSpotInfo { FIntPoint Coord; ESotaDirection RequiredOpDir; };
    TArray<FSpotInfo> PotentialSpots;
    
    for (const auto& Elem : RoomDataMap)
    {
        if (Elem.Value.Type != ERoomType::Normal && Elem.Value.Type != ERoomType::Start) continue;

        FIntPoint Coord = Elem.Key;
        ARoomBase* ParentCDO = Cast<ARoomBase>(Elem.Value.RoomClass->GetDefaultObject());
        if (!ParentCDO) continue;

        FIntPoint Neighbors[4] = { FIntPoint(Coord.X+1, Coord.Y), FIntPoint(Coord.X-1, Coord.Y), FIntPoint(Coord.X, Coord.Y+1), FIntPoint(Coord.X, Coord.Y-1) };

        for (FIntPoint NeighborCoord : Neighbors)
        {
            ESotaDirection DirToNeighbor = GetDirectionFromVector(NeighborCoord - Coord);

            // 해당 칸이 비어있고, 주변에 내 부모밖에 없으며, 부모 방이 나를 향해 문을 열 수 있을 때만 후보가 됩니다.
            if (!RoomDataMap.Contains(NeighborCoord) && GetNeighborCount(NeighborCoord) == 1 && ParentCDO->IsConnectorPlaceable(DirToNeighbor))
            {
                PotentialSpots.Add({NeighborCoord, GetOppositeDirection(DirToNeighbor)});
            }
        }
    }

    if (PotentialSpots.Num() > 0)
    {
        int32 SelectedIndex = 0;
        
        // ... (이전에 있던 Config.bFurthestSpawn 최장거리 계산 로직은 그대로 유지) ...
        if (Config.bFurthestSpawn)
        {
            int32 MaxDistance = -1;
            TArray<int32> MaxDistanceIndices; 

            for (int32 i = 0; i < PotentialSpots.Num(); ++i)
            {
                int32 GridDist = FMath::Abs(PotentialSpots[i].Coord.X) + FMath::Abs(PotentialSpots[i].Coord.Y);
                if (GridDist > MaxDistance)
                {
                    MaxDistance = GridDist;
                    MaxDistanceIndices.Empty(); 
                    MaxDistanceIndices.Add(i);
                }
                else if (GridDist == MaxDistance)
                {
                    MaxDistanceIndices.Add(i); 
                }
            }
            SelectedIndex = MaxDistanceIndices[FMath::RandRange(0, MaxDistanceIndices.Num() - 1)];
        }
        else
        {
            SelectedIndex = FMath::RandRange(0, PotentialSpots.Num() - 1);
        }

        FSpotInfo FinalSpot = PotentialSpots[SelectedIndex];
        
        // 보스방도 부모를 향해 문이 뚫려있는 클래스만 가져옵니다!
        TSubclassOf<ARoomBase> CompatibleClass = GetCompatibleRoomClass(RoomType, FinalSpot.RequiredOpDir);
        
        RoomDataMap.Add(FinalSpot.Coord, FRoomStatus(FinalSpot.Coord, RoomType, CompatibleClass));
        return true; 
    }
    return false; 
}

// ==============================================================================
// 2. 방 스폰 (로컬 방향 회전 보정 및 이벤트 구독)
// ==============================================================================
void ADungeonManager::SpawnRooms()
{
    SpawnedRoomMap.Empty();
    if (RoomPresets.Num() == 0) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (auto& Elem : RoomDataMap)
    {
        FIntPoint Coord = Elem.Key;
        ERoomType Type = Elem.Value.Type;
        TSubclassOf<ARoomBase> SelectedClass = Elem.Value.RoomClass;

        if (!SelectedClass) continue;

        FVector SpawnLocation(Coord.X * RoomGridSize, Coord.Y * RoomGridSize, 0.0f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        bool bIsDeadEndRoom = false;
        if (RoomPresets.Contains(Type) && Type != ERoomType::Start)
        {
            bIsDeadEndRoom = RoomPresets[Type].bIsConnectedByOneConnector;
        }

        if (bIsDeadEndRoom)
        {
            FIntPoint Neighbors[4] = { FIntPoint(Coord.X+1, Coord.Y), FIntPoint(Coord.X-1, Coord.Y), FIntPoint(Coord.X, Coord.Y+1), FIntPoint(Coord.X, Coord.Y-1) };

            for (int32 i = 0; i < 4; i++)
            {
                if (RoomDataMap.Contains(Neighbors[i]))
                {
                    FIntPoint Diff = Neighbors[i] - Coord;
                    if (Diff.X == 1)  SpawnRotation.Yaw = 0.0f;   
                    if (Diff.X == -1) SpawnRotation.Yaw = 180.0f; 
                    if (Diff.Y == 1)  SpawnRotation.Yaw = 90.0f;  
                    if (Diff.Y == -1) SpawnRotation.Yaw = -90.0f; 
                    break; 
                }
            }
        }

        ARoomBase* NewRoom = GetWorld()->SpawnActor<ARoomBase>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (NewRoom)
        {
            SpawnedRoomMap.Add(Coord, NewRoom);
            NewRoom->OnPlayerEnteredRoom.AddDynamic(this, &ADungeonManager::HandleRoomExplored);
        }
    }
}

// ==============================================================================
// 미니맵 갱신 콜백 (델리게이트 브로드캐스트)
// ==============================================================================
void ADungeonManager::HandleRoomExplored(ARoomBase* ExploredRoom)
{
    if (!ExploredRoom) return;

    const FIntPoint* FoundCoord = SpawnedRoomMap.FindKey(ExploredRoom);
    if (FoundCoord)
    {
        CurrentPlayerCoordinate = *FoundCoord;
        if (RoomDataMap.Contains(CurrentPlayerCoordinate))
        {
            RoomDataMap[CurrentPlayerCoordinate].bIsPlayerVisited = true;
        }

        UE_LOG(LogTemp, Log, TEXT("DungeonGenerator: Player is now in Room %s"), *CurrentPlayerCoordinate.ToString());
        
        // 미니맵 UI 갱신 신호 송출
        OnPlayerMovedRoom.Broadcast();
    }
}

// ==============================================================================
// 3. 커넥터(문) 및 빈 공간(Filler) 스폰 (갈등 조정 포함)
// ==============================================================================
void ADungeonManager::SpawnDoorsAndLink()
{
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

        // 허공을 막기 위해 4방향 전수 검사
        ESotaDirection AllDirs[] = { ESotaDirection::Forward, ESotaDirection::Backward, ESotaDirection::Right, ESotaDirection::Left };

        for (ESotaDirection Dir : AllDirs)
        {
            FIntPoint NeighborCoord = GetNeighborCoordinate(MyCoord, Dir);
            
            // 1. 이웃이 없으면 Filler로 막음
            if (!SpawnedRoomMap.Contains(NeighborCoord)) 
            {
                MyRoom->SpawnFiller(Dir);
                continue;
            }

            ARoomBase* NeighborRoom = SpawnedRoomMap[NeighborCoord];
            ERoomType NeighborType = RoomDataMap[NeighborCoord].Type;

            bool bNeighborIsSpecial = false;
            if (RoomPresets.Contains(NeighborType) && NeighborType != ERoomType::Start)
            {
                bNeighborIsSpecial = RoomPresets[NeighborType].bIsConnectedByOneConnector;
            }

            // 2. 특수방 규칙에 의해 연결이 차단되는지 검사
            bool bIsConnectionBlocked = false;

            if (bIsMyRoomSpecial)
            {
                FVector WorldDirVec = FVector(NeighborCoord.X - MyCoord.X, NeighborCoord.Y - MyCoord.Y, 0.0f);
                if (!WorldDirVec.GetSafeNormal().Equals(MyRoom->GetActorForwardVector(), 0.1f)) bIsConnectionBlocked = true;
            }
            if (bNeighborIsSpecial)
            {
                FVector WorldToMeVec = FVector(MyCoord.X - NeighborCoord.X, MyCoord.Y - NeighborCoord.Y, 0.0f);
                if (!WorldToMeVec.GetSafeNormal().Equals(NeighborRoom->GetActorForwardVector(), 0.1f)) bIsConnectionBlocked = true;
            }

            if (bIsConnectionBlocked)
            {
                MyRoom->SpawnFiller(Dir);
                continue;
            }

            // 3. 중복 생성 방지를 위해 Forward, Right 일 때만 주도적으로 문 스폰
            if (Dir == ESotaDirection::Forward || Dir == ESotaDirection::Right)
            {
                ESotaDirection OpDir = GetOppositeDirection(Dir);
                
                // 갈등 조정 로직: 양쪽 방의 선호 클래스를 확인
                TSubclassOf<ARoomConnector> MyConnectorClass = MyRoom->GetConnectorClass(Dir);
                TSubclassOf<ARoomConnector> NeighborConnectorClass = NeighborRoom->GetConnectorClass(OpDir);
                TSubclassOf<ARoomConnector> FinalClassToSpawn = MyConnectorClass;

                // 특수방의 문 디자인을 최우선으로 적용
                if (bNeighborIsSpecial && !bIsMyRoomSpecial) FinalClassToSpawn = NeighborConnectorClass;
                else if (bIsMyRoomSpecial && !bNeighborIsSpecial) FinalClassToSpawn = MyConnectorClass;
                else if (!FinalClassToSpawn && NeighborConnectorClass) FinalClassToSpawn = NeighborConnectorClass;

                // 최종 결정된 클래스로 스폰 및 양쪽 방에 등록
                ARoomConnector* SpawnedConnector = MyRoom->SpawnConnector(Dir, FinalClassToSpawn);
                if (SpawnedConnector)
                {
                    NeighborRoom->RegisterConnector(OpDir, SpawnedConnector);
                }
            }
        }
    }
}

// ==============================================================================
// 헬퍼 함수들
// ==============================================================================

int32 ADungeonManager::GetNeighborCount(FIntPoint Coord)
{
    int32 Count = 0;
    if (RoomDataMap.Contains(FIntPoint(Coord.X + 1, Coord.Y))) Count++;
    if (RoomDataMap.Contains(FIntPoint(Coord.X - 1, Coord.Y))) Count++;
    if (RoomDataMap.Contains(FIntPoint(Coord.X, Coord.Y + 1))) Count++;
    if (RoomDataMap.Contains(FIntPoint(Coord.X, Coord.Y - 1))) Count++;
    return Count;
}

FIntPoint ADungeonManager::GetNeighborCoordinate(FIntPoint Current, ESotaDirection Direction)
{
    switch (Direction)
    {
        case ESotaDirection::Forward:  return FIntPoint(Current.X + 1, Current.Y);
        case ESotaDirection::Right:    return FIntPoint(Current.X, Current.Y + 1);
        case ESotaDirection::Backward: return FIntPoint(Current.X - 1, Current.Y);
        case ESotaDirection::Left:     return FIntPoint(Current.X, Current.Y - 1);
        default: return Current;
    }
}

ESotaDirection ADungeonManager::GetOppositeDirection(ESotaDirection Direction)
{
    switch (Direction)
    {
        case ESotaDirection::Forward:  return ESotaDirection::Backward;
        case ESotaDirection::Right:    return ESotaDirection::Left;
        case ESotaDirection::Backward: return ESotaDirection::Forward;
        case ESotaDirection::Left:     return ESotaDirection::Right;
        default: return ESotaDirection::Forward;
    }
}

TSubclassOf<ARoomBase> ADungeonManager::GetRandomRoomClass(ERoomType Type)
{
    if (RoomPresets.Contains(Type))
    {
        const TArray<TSubclassOf<ARoomBase>>& Candidates = RoomPresets[Type].Variations;
        if (Candidates.Num() > 0)
        {
            return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
        }
    }

    if (Type != ERoomType::Normal && RoomPresets.Contains(ERoomType::Normal))
    {
        return GetRandomRoomClass(ERoomType::Normal);
    }
    return nullptr;
}

ESotaDirection ADungeonManager::GetDirectionFromVector(FIntPoint Vector)
{
    if (Vector == FIntPoint(1, 0)) return ESotaDirection::Forward;
    if (Vector == FIntPoint(-1, 0)) return ESotaDirection::Backward;
    if (Vector == FIntPoint(0, 1)) return ESotaDirection::Right;
    if (Vector == FIntPoint(0, -1)) return ESotaDirection::Left;
    return ESotaDirection::Empty;
}

TSubclassOf<ARoomBase> ADungeonManager::GetCompatibleRoomClass(ERoomType Type, ESotaDirection RequiredDirection)
{
    if (RoomPresets.Contains(Type))
    {
        const TArray<TSubclassOf<ARoomBase>>& Candidates = RoomPresets[Type].Variations;
        TArray<TSubclassOf<ARoomBase>> ValidCandidates;

        for (TSubclassOf<ARoomBase> Class : Candidates)
        {
            if (Class)
            {
                // CDO(Class Default Object)를 가져와서 기획자가 설정한 플래그를 확인합니다.
                if (ARoomBase* CDO = Cast<ARoomBase>(Class->GetDefaultObject()))
                {
                    if (CDO->IsConnectorPlaceable(RequiredDirection))
                    {
                        ValidCandidates.Add(Class); // 이 방향에 문이 달릴 수 있는 방만 합격!
                    }
                }
            }
        }

        // 합격한 후보군 중에서 랜덤으로 하나를 뽑습니다.
        if (ValidCandidates.Num() > 0)
        {
            return ValidCandidates[FMath::RandRange(0, ValidCandidates.Num() - 1)];
        }
    }

    // 만약 기획 오류로 맞는 방이 없다면, 일반 방(Normal) 중에서라도 호환되는 걸 찾아봅니다. (안전장치)
    if (Type != ERoomType::Normal && RoomPresets.Contains(ERoomType::Normal))
    {
        return GetCompatibleRoomClass(ERoomType::Normal, RequiredDirection);
    }

    // 최후의 수단: 어쩔 수 없이 아무거나 리턴 (기획 데이터 점검 필요)
    return GetRandomRoomClass(Type);
}