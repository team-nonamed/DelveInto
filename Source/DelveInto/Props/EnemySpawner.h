// EnemySpawner.h

#pragma once

#include "CoreMinimal.h"

#include "Entities/Hosts/DefaultEnemyCharacter.h"
#include "Entities/Hosts/FPSPlayer.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class ARoomBase;

UCLASS()
class DELVEINTO_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	UPROPERTY(VisibleAnywhere, Category="Spawn")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<ADefaultEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, Category="Spawn")
	int32 SpawnCount = 3;

	UPROPERTY(EditAnywhere, Category="Spawn")
	float SpawnRadius = 300.f;

	// 이 스포너가 속한 Room (BP에서 세팅하거나, 자동으로 찾아도 됨)
	UPROPERTY(EditAnywhere, Category="Room")
	ARoomBase* OwningRoom;

	bool bHasSpawned = false;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void HandleOnPlayerEnterRoom(AFPSPlayer* PlayerActor);

	void SpawnEnemiesInternal(AFPSPlayer* PlayerActor);
};
