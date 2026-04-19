// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleEnemySpawner.h"

#include "PaperFlipbookComponent.h"
#include "PaperSprite.h"
#include "Components/CapsuleComponent.h"


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


void USimpleEnemySpawner::UpdatePreviewRenderer_Implementation()
{
#if WITH_EDITOR
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
    
    if (DefaultEnemyData && DefaultEnemyData->IdleFlipbook)
    {
       // 1. 뼈대가 되는 플립북 세팅
       UPaperFlipbook* Flipbook = DefaultEnemyData->IdleFlipbook;
       PreviewFlipbook->SetFlipbook(Flipbook);

       // 2. CDO에서 캡슐의 높이 가져오기
       float CapHalfHeight = DefaultEnemyData->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
       
       // 3. 플립북의 첫 번째 프레임(스프라이트)에서 실제 렌더링 크기 추출
    	float SpriteWorldHeight = 1.0f; // 0으로 나누기 방지용 기본값
    	UPaperSprite* FirstSprite = Flipbook->GetSpriteAtFrame(0);
       
    	if (FirstSprite)
    	{
    		// [수정] 변수 이름을 Bounds에서 SpriteBounds로 변경합니다!
    		FBoxSphereBounds SpriteBounds = FirstSprite->GetRenderBounds();
    		SpriteWorldHeight = SpriteBounds.BoxExtent.Z * 2.0f; 
    	}

       // 4. ADelveEnemy::OnConstruction과 완벽히 동일한 스케일 계산 로직
       if (DefaultEnemyData->bAutoResizeToCapsule && SpriteWorldHeight > 1.0f)
       {
           float CapsuleTotalHeight = CapHalfHeight * 2.0f;
           float NewScale = CapsuleTotalHeight / SpriteWorldHeight;
           
           PreviewFlipbook->SetRelativeScale3D(FVector(NewScale, NewScale, NewScale));
       }
       else
       {
           // 자동 조절을 안 쓴다면 기본 객체에 세팅된 스케일을 그대로 씁니다.
           PreviewFlipbook->SetRelativeScale3D(DefaultEnemyData->EnemyFlipbook->GetRelativeScale3D());
       }

       // 5. 위치와 회전 맞추기 (캡슐 바닥으로 내리고 -90도 회전)
       PreviewFlipbook->SetRelativeLocation(FVector(0.0f, 0.0f, -CapHalfHeight));
       PreviewFlipbook->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }
    else
    {
       PreviewFlipbook->SetFlipbook(nullptr);
       PreviewFlipbook->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    }
#endif
}

#if WITH_EDITOR
void USimpleEnemySpawner::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Execute_UpdatePreviewRenderer(this);

}
#endif
