// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/RoomBase.h"

ARoomBase::ARoomBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

/**
 * Room 입장 Trigger가 Trig 되었을 때 호출하는 함수
 * @param Player `Room`에 입장한 PlayableCharacter
 */
void ARoomBase::NotifyPlayerEntered(APlayableCharacter* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("Player does not exist"));
		return;
	}
		

	int32& Count = PlayerOverlapCounts.FindOrAdd(Player);

	Count++;

	// 0 → 1 되는 순간이 “실제 입장”
	if (Count == 1)
	{
		UE_LOG(LogTemp, Display, TEXT("Success"));
		OnPlayerEnterRoom.Broadcast(Player);
	}

	UE_LOG(LogTemp, Display, TEXT("%i"), Count);
}

/**
 * Room 퇴장 Trigger가 Trig 되었을 때 호출하는 함수
 * @param Player `Room`에서 나간 PlayableCharacter
 */
void ARoomBase::NotifyPlayerExited(APlayableCharacter* Player)
{
	if (!Player)
		return;

	int32* CountPtr = PlayerOverlapCounts.Find(Player);
	if (!CountPtr)
	{
		UE_LOG(LogTemp, Display, TEXT("?"))
		// 대응되는 Enter가 없었거나, 이미 지워진 경우
		return;
	}

	(*CountPtr)--;

	if (*CountPtr <= 0)
	{
		UE_LOG(LogTemp, Display, TEXT("나가다!"))
		// 완전히 방에서 나간 순간
		PlayerOverlapCounts.Remove(Player);
		OnPlayerExitRoom.Broadcast(Player);
	}
}


// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("Room이 초기화 되었습니다! %s"), *GetName());
}

// // Called every frame
// void ARoomBase::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// }

