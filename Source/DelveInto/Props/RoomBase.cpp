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
	OnPlayerEnterRoom.Broadcast(Player);
}

/**
 * Room 퇴장 Trigger가 Trig 되었을 때 호출하는 함수
 * @param Player `Room`에서 나간 PlayableCharacter
 */
void ARoomBase::NotifyPlayerExited(APlayableCharacter* Player)
{
	OnPlayerExitRoom.Broadcast(Player);
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

