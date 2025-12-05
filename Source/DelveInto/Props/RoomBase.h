// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/PlayableCharacter.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

/**
 * 방의 연결 상태를 나타내는 구조체
 */
USTRUCT(BlueprintType)
struct FRoomConnectionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly)
	bool bIsConnectedToNorth;
	
	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly)
	bool bIsConnectedToSouth;
	
	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly)
	bool bIsConnectedToEast;

	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly)
	bool bIsConnectedToWest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsConnectedToUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsConnectedToDown;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEnterRoom, APlayableCharacter*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerExitRoom, APlayableCharacter*, Player);

/**
 * 방의 기본 Actor 클래스
 */
UCLASS()
class DELVEINTO_API ARoomBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Ticking을 사용하지 않는 것을 명시적으로 지정
	ARoomBase();

	// 방의 연결 상태 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Room|Info")
	FRoomConnectionInfo RoomConnectionInfo = FRoomConnectionInfo();

	// Subscriber들이 Binding할 Player 입장 Event
	UPROPERTY(BlueprintAssignable, Category="Room|Event")
	FOnPlayerEnterRoom OnPlayerEnterRoom;

	// Subscriber들이 Binding할 Player 퇴장 Event
	UPROPERTY(BlueprintAssignable, Category="Room|Event")
	FOnPlayerExitRoom OnPlayerExitRoom;

	// Room 입장 Trigger가 Trig 되었을 때 호출하는 함수
	UFUNCTION(BlueprintCallable, Category="Room|Event")
	void NotifyPlayerEntered(APlayableCharacter* Player);

	// Room 퇴장 Trigger가 Trig 되었을 때 호출하는 함수
	UFUNCTION(BlueprintCallable, Category="Room|Event")
	void NotifyPlayerExited(APlayableCharacter* Player);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// public:	
// 	// Called every frame
// 	virtual void Tick(float DeltaTime) override;

private:
	// 플레이어별 “겹침 카운트”
	UPROPERTY()
	TMap<TWeakObjectPtr<APlayableCharacter>, int32> PlayerOverlapCounts;
};
