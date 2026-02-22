// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleEnemySpawner.h"

#include "PaperFlipbookComponent.h"


// Sets default values for this component's properties
USimpleEnemySpawner::USimpleEnemySpawner()
{
	PrimaryComponentTick.bCanEverTick = false;

	PreviewFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PreviewFlipbook"));
	PreviewFlipbook->SetupAttachment(this);

	PreviewFlipbook->SetCollisionProfileName(TEXT("NoCollision")); // 충돌 끄기
	PreviewFlipbook->SetGenerateOverlapEvents(false);             // 오버랩 끄기
	PreviewFlipbook->SetHiddenInGame(true);                       // 실제 게임 시작하면 숨기기 (선택)
	PreviewFlipbook->CastShadow = false;                          // 그림자 끄기 (최적화)

	// 몬스터가 보통 정면(-90도)을 보므로 회전값 미리 보정
	PreviewFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

// Called when the game starts
void USimpleEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

ADelveEnemy* USimpleEnemySpawner::SpawnEnemy_Implementation()
{
	if (!EnemyClassToSpawn)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FTransform SpawnTransform = GetComponentTransform();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADelveEnemy* SpawnedActor = World-> SpawnActor<ADelveEnemy>(EnemyClassToSpawn, SpawnTransform, SpawnParameters);
	DestroyComponent();

	return SpawnedActor;
}

#if WITH_EDITOR
void USimpleEnemySpawner::UpdatePreviewRenderer_Implementation()
{
	if (!PreviewFlipbook)
	{
		return;
	}

	if (!EnemyClassToSpawn)
	{
		PreviewFlipbook->SetFlipbook(nullptr);
		return;
	}

	ADelveEnemy* DefaultEnemyData = EnemyClassToSpawn->GetDefaultObject<ADelveEnemy>();
	
	UPaperFlipbook* Flipbook = DefaultEnemyData->IdleFlipbook;

	if (Flipbook)
	{
		PreviewFlipbook->SetFlipbook(Flipbook);
	}
	else
	{
		PreviewFlipbook->SetFlipbook(nullptr);
	}
}

void USimpleEnemySpawner::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Execute_UpdatePreviewRenderer(this);
}

#endif