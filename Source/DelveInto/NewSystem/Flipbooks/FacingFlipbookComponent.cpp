#include "FacingFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

UFacingFlipbookComponent::UFacingFlipbookComponent()
{
	// 매 프레임마다 회전해야 하므로 Tick을 켭니다.
	PrimaryComponentTick.bCanEverTick = true;
    
	// [핵심] 카메라가 먼저 움직이고 난 뒤에 판때기를 돌려야 화면 떨림(Jittering)이 없습니다.
	PrimaryComponentTick.TickGroup = TG_PostPhysics; 
}

void UFacingFlipbookComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFacingFlipbookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1. 플레이어 카메라 매니저 가져오기
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager) return;

	// 2. 방향 벡터 구하기 (목표 위치 - 내 위치)
	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector MyLocation = GetComponentLocation();
    
	FVector DirectionToCamera = CameraLocation - MyLocation;

	// 3. (옵션) 수직 회전 방지
	if (bLockPitchAndRoll)
	{
		DirectionToCamera.Z = 0.0f; // Z축 변위를 무시하여 항상 땅에 수직으로 서 있게 함
	}

	// 방향 벡터가 너무 짧아 0으로 나누어지는 오류(NaN) 방지
	if (DirectionToCamera.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		// 4. 방향 벡터를 기반으로 회전값 생성
		FRotator TargetRotation = DirectionToCamera.Rotation();

		// 5. 보정값(Offset) 더해주기
		TargetRotation += RotationOffset;

		// 6. 월드 회전 적용
		SetWorldRotation(TargetRotation);
	}
}