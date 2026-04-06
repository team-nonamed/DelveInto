// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomConnector.h"
#include "UObject/Object.h"
#include "ConnectorSpawner.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Spawner), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UConnectorSpawner : public USceneComponent
{
	GENERATED_BODY()

#pragma region Properties

#pragma region ConnectorClass
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Connector")
	TSubclassOf<ARoomConnector> ConnectorClass;

public:
	UFUNCTION(BlueprintPure)
	TSubclassOf<ARoomConnector> GetConnectorClass() const
	{
		return ConnectorClass;
	}

	
#pragma endregion

#pragma endregion
};
