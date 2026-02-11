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

// 1. 방 배치 알고리즘 (Random Walk)
void ADungeonGenerator::CreateLayout()
{
    RoomDataMap.Empty();

    // 시작 방 (0,0)
    FIntPoint CurrentCoord(0, 0);
    RoomDataMap.Add(CurrentCoord, FRoomData(CurrentCoord, ERoomType::Start));

    int32 MaxIterations = MaxRoomCount * 10; // 무한 루프 방지용 안전장치
    int32 CurrentIterations = 0;

    // 목표 개수만큼 방 생성
    while (RoomDataMap.Num() < MaxRoomCount && CurrentIterations < MaxIterations)
    {
        CurrentIterations++;

        // 4방향 중 랜덤 선택
        int32 RandDir = FMath::RandRange(0, 3);
        FIntPoint NextCoord = CurrentCoord;

        switch (RandDir)
        {
            case 0: NextCoord.X += 1; break; // Forward (+X)
            case 1: NextCoord.Y += 1; break; // Right   (+Y)
            case 2: NextCoord.X -= 1; break; // Backward (-X)
            case 3: NextCoord.Y -= 1; break; // Left    (-Y)
        }

        // 아직 방이 없는 곳이라면 추가
        if (!RoomDataMap.Contains(NextCoord))
        {
            ERoomType NewType = ERoomType::Normal;
            // (선택) 마지막 방을 보스 방으로 설정하는 로직 등 추가 가능
            
            RoomDataMap.Add(NextCoord, FRoomData(NextCoord, NewType));
            CurrentCoord = NextCoord; // 이동
        }
        else
        {
            // 이미 있으면 그 위치에서 다시 탐색 (확률적 분기)
            CurrentCoord = NextCoord; 
        }
    }
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