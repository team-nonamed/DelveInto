#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DelveRoomBase.generated.h"

// 방의 종류를 구분하기 위한 열거형 (미니맵 표시 등에 사용)
UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Normal,
    Treasure,
    Boss,
    Shop
};

UCLASS()
class DELVEINTO_API ADelveRoomBase : public AActor
{
    GENERATED_BODY()
    
public:    
    ADelveRoomBase();

protected:
    virtual void BeginPlay() override;

public:
    // --- 컴포넌트 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    class UBoxComponent* RoomTrigger; // 플레이어 진입 감지용

    // 문이 생성될 위치 (상하좌우) - 화살표 컴포넌트로 위치 지정
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    class UArrowComponent* DoorPoint_Top;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    class UArrowComponent* DoorPoint_Bottom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    class UArrowComponent* DoorPoint_Left;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    class UArrowComponent* DoorPoint_Right;

    // --- 설정 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Settings")
    ERoomType RoomType = ERoomType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Settings")
    bool bIsCleared = false;

    // --- 핵심 로직 (자식들이 오버라이드할 함수들) ---
    
    // 플레이어가 처음 들어왔을 때 실행 (예: 몬스터 스폰, 문 잠그기)
    virtual void OnPlayerEnterRoom();

    // 방 클리어 조건 체크 (몬스터 죽을 때마다 호출)
    virtual void CheckRoomCleared();

    // --- 문 제어 ---
    // 실제 문 액터들을 저장할 배열
    UPROPERTY(VisibleAnywhere, Category = "Room")
    TArray<class ADelveDoor*> SpawnedDoors;

    UFUNCTION(BlueprintCallable)
    void LockDoors(); // 문 닫고 못 나가게

    UFUNCTION(BlueprintCallable)
    void UnlockDoors(); // 문 열기

protected:
    // 트리거 오버랩 이벤트
    UFUNCTION()
    void OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    bool bHasEntered = false; // 이미 방문했는지 체크
};