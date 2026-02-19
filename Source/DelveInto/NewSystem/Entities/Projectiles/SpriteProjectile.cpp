#include "SpriteProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASpriteProjectile::ASpriteProjectile()
{
	// 1. 매 프레임 카메라를 바라보도록 업데이트해야 하므로 Tick 활성화
	PrimaryActorTick.bCanEverTick = true;

	// 2. 스프라이트 컴포넌트 생성 및 부모의 루트 컴포넌트(CollisionComp)에 부착
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->SetCollisionProfileName(TEXT("NoCollision"));

	// 3. 부모 클래스에서 만들어둔 이동 컴포넌트의 설정 변경
	// 스프라이트가 회전을 직접 통제할 것이므로, 이동 방향으로 자동 회전하는 기능을 끕니다.
	if (ProjectileMovement)
	{
		ProjectileMovement->bRotationFollowsVelocity = false;
	}
}

void ASpriteProjectile::BeginPlay()
{
	// 부모의 BeginPlay(속도 동기화, MuzzleFlash 등)를 그대로 실행
	Super::BeginPlay();
}

void ASpriteProjectile::Tick(float DeltaTime)
{
	// 부모에 Tick 기능이 추가될 것을 대비해 호출
	Super::Tick(DeltaTime);

	if (!SpriteComponent) return;

	APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CamManager) return;

	// 목표 지점(카메라)과 내 위치
	FVector CamLoc = CamManager->GetCameraLocation();
	FVector MyLoc = GetActorLocation();

	// 방향 벡터 구하기
	FVector LookDirVector = CamLoc - MyLoc;

	// [핵심] 수평(바닥 정사영) 벡터로 만들기 위해 Z축 값을 0으로 고정
	LookDirVector.Z = 0.0f;

	if (!LookDirVector.IsNearlyZero())
	{
		// 방향 벡터를 회전값으로 변환
		FRotator TargetRotation = LookDirVector.Rotation();
        
		// (참고) 언리얼 스프라이트는 기본적으로 X축 정면을 봅니다.
		// 에셋에 따라 카메라를 똑바로 보게 하려면 여기서 Yaw 축을 -90도 회전시켜야 할 수도 있습니다.
		TargetRotation.Yaw -= 90.0f;

		// 액터 전체(충돌체 포함)가 아닌 '스프라이트 컴포넌트만' 회전시킵니다.
		SpriteComponent->SetWorldRotation(TargetRotation);
	}
}