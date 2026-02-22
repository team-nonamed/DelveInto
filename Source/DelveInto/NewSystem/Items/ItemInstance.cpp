#include "ItemInstance.h"
#include "Components/SphereComponent.h"
#include "PaperSpriteComponent.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h"

AItemInstance::AItemInstance()
{
    PrimaryActorTick.bCanEverTick = false;

    // ==========================================
    // [C++ 중앙 제어] 여기서 모든 수치를 관리합니다!
    // ==========================================
    float PhysicsRadius = 15.0f; // 땅에 구르는 물리적 크기
    float PickupRadius = 80.0f;  // 플레이어가 자석처럼 먹을 수 있는 널널한 판정 범위
    FVector VisualScale = FVector(0.3f, 0.3f, 0.3f); // 스프라이트/플립북의 화면상 크기
    // ==========================================

    // 1. 물리 콜라이더 세팅 (구르기 담당)
    PhysicsCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PhysicsCollider"));
    RootComponent = PhysicsCollider;
    PhysicsCollider->InitSphereRadius(PhysicsRadius);
    
    PhysicsCollider->SetSimulatePhysics(true);
    PhysicsCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PhysicsCollider->SetCollisionObjectType(ECC_PhysicsBody);

    // 회전 잠금 (마구 구르더라도 이미지는 돌아가지 않게 꽉 잡아줌)
    PhysicsCollider->GetBodyInstance()->bLockXRotation = true;
    PhysicsCollider->GetBodyInstance()->bLockYRotation = true;
    PhysicsCollider->GetBodyInstance()->bLockZRotation = true;

    // 길막 방지
    PhysicsCollider->SetCollisionResponseToAllChannels(ECR_Block);
    PhysicsCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    PhysicsCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    PhysicsCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

    // 2. 획득 트리거 세팅 (널널한 판정 담당)
    PickupTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PickupTrigger"));
    PickupTrigger->SetupAttachment(RootComponent);
    PickupTrigger->InitSphereRadius(PickupRadius);
    
    PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어만 감지
    PickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &AItemInstance::OnBeginOverlap);

    // 3. 시각 효과 (크기 일괄 조절)
    ItemSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ItemSprite"));
    ItemSprite->SetupAttachment(RootComponent);
    ItemSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
    ItemSprite->SetRelativeScale3D(VisualScale);
}

void AItemInstance::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ItemData가 할당되지 않은 오류 상태라면 무시
	if (!ItemData) return;

	if (OtherActor && OtherActor->IsA(ADelveCharacter::StaticClass()))
	{
		ADelveCharacter* PlayerChar = Cast<ADelveCharacter>(OtherActor);
		UInventoryHandler* InvHandler = PlayerChar->FindComponentByClass<UInventoryHandler>();
        
		if (InvHandler)
		{
			// 1. 인벤토리에 아이템 추가를 시도하고, 다 못 넣고 남은 개수를 반환받습니다.
			int32 LeftoverAmount = InvHandler->AddItem(ItemData, Amount);
            
			// 2. 남은 개수가 없다면 (전부 획득했다면) 액터를 파괴합니다.
			if (LeftoverAmount <= 0)
			{
				// TODO: 획득 효과음/이펙트 재생 로직 추가
				Destroy();
			}
			// 3. 인벤토리가 꽉 차서 일부를 남겼다면, 떨어진 개수를 갱신합니다.
			else
			{
				Amount = LeftoverAmount;
				UE_LOG(LogTemp, Warning, TEXT("인벤토리 공간 부족으로 %d개를 남겼습니다."), Amount);
			}
		}
	}
}

void AItemInstance::InitializeItem(UItemData* InItemData, int32 InAmount)
{
	ItemData = InItemData;
	Amount = InAmount;

	if (ItemData && ItemSprite && ItemData->ItemIcon)
	{
		// 1. 스프라이트 이미지 세팅
		ItemSprite->SetSprite(ItemData->ItemIcon);

		// 2. [핵심] 스프라이트의 실제 렌더링 사이즈(높이)를 가져옵니다.
		// GetRenderBounds()는 에셋의 픽셀 크기와 Pixels Per Unit 세팅이 모두 반영된 실제 크기입니다.
		FVector SpriteSize = ItemData->ItemIcon->GetRenderBounds().GetBox().GetSize();
        
		// Z축(높이)이 0보다 클 때만 계산 (안전장치)
		if (SpriteSize.Z > 0.0f)
		{
			// 3. 목표 높이는 물리 콜라이더의 지름(반지름 * 2)입니다.
			// (만약 아이템이 콜라이더보다 1.5배 정도 더 컸으면 좋겠다! 하시면 2.0f 대신 3.0f를 곱하시면 됩니다)
			float TargetHeight = PhysicsCollider->GetUnscaledSphereRadius() * 2.0f;

			// 4. 새로운 스케일 비율 계산 (목표 높이 / 실제 이미지 높이)
			float NewScale = TargetHeight / SpriteSize.Z;

			// 5. 스케일 적용 및 위치를 정중앙(Zero)으로 초기화!
			ItemSprite->SetRelativeScale3D(FVector(NewScale, NewScale, NewScale));
			ItemSprite->SetRelativeLocation(FVector::ZeroVector);
		}

		UE_LOG(LogTemp, Display, TEXT("%s 아이템이 스폰되며 크기가 자동 조절되었습니다!"), *ItemData->ItemName.ToString());
	}
}