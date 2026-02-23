#include "Chest.h"
#include "PaperSprite.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NewSystem/Utils/ItemUtil.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. Root를 SceneComponent로 설정 (이 자리가 0,0,0 바닥 지점)
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	// 2. 박스를 루트에 부착
	InteractCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(RootComponent);
    
	// 초기 박스 크기 설정
	FVector InitialExtent(50.f, 50.f, 50.f);
	InteractCollision->SetBoxExtent(InitialExtent);
    
	// [핵심] 박스의 중심을 자기 높이의 절반만큼 위로 올림 -> 박스 바닥이 0점에 위치함
	InteractCollision->SetRelativeLocation(FVector(0, 0, InitialExtent.Z));
	InteractCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 3. 스프라이트를 루트에 부착 (박스와 형제 관계)
	ChestSprite = CreateDefaultSubobject<UFacingSpriteComponent>(TEXT("ChestSprite"));
	ChestSprite->SetupAttachment(RootComponent);
    
	// 그림은 바닥(0,0,0)에서 바로 시작하도록 설정
	ChestSprite->SetRelativeLocation(FVector::ZeroVector);
}

void AChest::BeginPlay()
{
	Super::BeginPlay();

	// 시작할 때 닫힌 상자 플립북으로 세팅
	if (ClosedSprite)
	{
		ChestSprite->SetSprite(ClosedSprite);
	}
}

void AChest::Interact_Implementation(AActor* Interactor)
{
	if (bIsOpen) return;
	bIsOpen = true;

	// 1. 애니메이션 및 사운드 재생 (기존 로직 유지)
	if (OpenedSprite) {
		ChestSprite->SetSprite(OpenedSprite);
	}
	if (OpenSound) {
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}

	// 2. 역동적인 아이템 스폰 로직
	if (LootTable.Num() > 0)
	{
		// [핵심] 상자 박스 콜리전의 정중앙 좌표 (World Space)를 가져옵니다.
		// 이전에 박스를 위로 Offset 시켰으므로, GetComponentLocation()이 바로 박스의 중심입니다.
		FVector SpawnCenter = InteractCollision->GetComponentLocation();

		for (const FLootEntry& Entry : LootTable)
		{
			// 확률 검사
			if (FMath::FRand() > Entry.DropChance) continue;

			int32 FinalAmount = FMath::RandRange(Entry.MinAmount, Entry.MaxAmount);
			if (FinalAmount <= 0) continue;

			// 아이템 스폰 (UItemUtil은 이전에 정의한 유틸리티를 사용한다고 가정)
			// SpawnDroppedItems 내부에서 스폰된 Actor들의 배열을 반환받거나, 
			// 직접 스폰 후 물리 힘을 가하는 처리가 필요합니다.
            
			// 예시: 개별 아이템을 스폰하면서 랜덤한 방향으로 튕겨내기
			for (int32 i = 0; i < FinalAmount; ++i)
			{
				// 스폰 지점으로부터 랜덤한 발사 벡터 계산 (위쪽 방향 위주로)
				FVector LaunchDir = FMath::VRandCone(FVector::UpVector, FMath::DegreesToRadians(45.0f));
				float RandomForce = FMath::FRandRange(ExplosionForce * 0.7f, ExplosionForce);
                
				// [참고] UItemUtil::SpawnDroppedItems가 내부적으로 Actor를 생성한다면 
				// 해당 Actor의 PrimitiveComponent에 Impulse를 가해야 합니다.
				// 여기서는 기존 Util 함수를 호출하되, 위치를 SpawnCenter로 전달합니다.
				UItemUtil::SpawnDroppedItems(this, Entry.ItemID, 1, SpawnCenter, LaunchDir * RandomForce);
			}
		}
	}
}

void AChest::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (InteractCollision && ChestSprite && ClosedSprite)
	{
		// 1. 현재 박스의 Extent(절반 크기) 가져오기
		FVector CurrentExtent = InteractCollision->GetUnscaledBoxExtent();

		// 2. [바닥 고정] 박스를 항상 자기 높이 절반만큼 위로 올림
		InteractCollision->SetRelativeLocation(FVector(0, 0, CurrentExtent.Z));

		// 3. 스프라이트 크기 자동 조절 (비율 유지)
		ChestSprite->SetSprite(ClosedSprite);
		FBoxSphereBounds SpriteBounds = ClosedSprite->GetRenderBounds();
		FVector RawSize = SpriteBounds.BoxExtent * 2.0f;

		float SpriteW = FMath::Max(RawSize.X, RawSize.Y);
		float SpriteH = RawSize.Z;

		if (SpriteW > 0.1f && SpriteH > 0.1f)
		{
			// 박스 전체 크기 (Extent * 2) 대비 배율 계산
			float ScaleW = (CurrentExtent.X * 2.0f) / SpriteW;
			float ScaleH = (CurrentExtent.Z * 2.0f) / SpriteH;
            
			float FinalScale = FMath::Min(ScaleW, ScaleH);
			ChestSprite->SetRelativeScale3D(FVector(FinalScale));
            
			// 그림도 바닥에 딱 붙도록 위치 조정 (그림 피벗이 중앙일 경우)
			// 만약 그림이 공중에 뜬다면 이 수치를 조절하세요.
			ChestSprite->SetRelativeLocation(FVector(0, 0, 0));
		}
	}
}