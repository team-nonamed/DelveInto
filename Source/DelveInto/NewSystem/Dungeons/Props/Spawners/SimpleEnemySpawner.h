// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NewSystem/Interfaces/EnemySpawner.h"
#include "SimpleEnemySpawner.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API USimpleEnemySpawner : public USceneComponent, public IEnemySpawner
{
	GENERATED_BODY()

public:
	USimpleEnemySpawner();

#pragma region Properties
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn Configs")
	TSubclassOf<ADelveEnemy> EnemyClassToSpawn;

	/**
	 * Editor 내에서 표시될 Enemy Flipbook
	 */
	UPROPERTY(VisibleAnywhere, Transient, Category = "Debug")
	class UPaperFlipbookComponent* PreviewFlipbook;
#pragma endregion

#pragma region Methods
protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void UpdatePreviewRenderer_Implementation() override;
	
public:
	virtual ADelveEnemy* SpawnEnemy_Implementation() override;
#pragma endregion
};
