#include "FacingSpriteComponent.h"
#include "Kismet/KismetMathLibrary.h"

UFacingSpriteComponent::UFacingSpriteComponent()
{
	// 컴포넌트가 Tick을 수행할 수 있도록 설정합니다.
	PrimaryComponentTick.bCanEverTick = true;
}

void UFacingSpriteComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFacingSpriteComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateFacingRotation();
}

void UFacingSpriteComponent::UpdateFacingRotation()
{
	if (!GetWorld()) return;

	// 1. 플레이어 카메라 매니저를 통해 현재 활성화된 카메라 위치를 가져옵니다.
	APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController() ? GetWorld()->GetFirstPlayerController()->PlayerCameraManager : nullptr;
	
	if (CameraManager)
	{
		FVector CameraLocation = CameraManager->GetCameraLocation();
		FVector ComponentLocation = GetComponentLocation();

		// 2. 두 지점 사이의 회전값 계산 (Z축 회전만 사용하여 꼿꼿이 서 있게 함)
		// 만약 공중에서도 나를 완전히 바라보게 하려면 Pitch값도 포함할 수 있습니다.
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(ComponentLocation, CameraLocation);
		
		// 2D 스프라이트의 정면 방향에 따라 +90 혹은 -90도 보정이 필요할 수 있습니다.
		FRotator FinalRot = FRotator(0.f, LookAtRot.Yaw - 90.f, 0.f);

		SetWorldRotation(FinalRot);
	}
}