// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Entity.generated.h"

/**
 *	Host(논리적 진입점) Actor.
 *		- 핸들러 컴포넌트를 소유/연결하고
 *		- InputController로부터 전달받은 입력을 Handler로 라우팅
 *		- IHurtHandler도 Actor 레벨에서 구현하고 내부 HealthHandler(미구현)로 위임하는 형태를 권장
 */
UCLASS(Abstract)
class DELVEINTO_API AEntity : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
