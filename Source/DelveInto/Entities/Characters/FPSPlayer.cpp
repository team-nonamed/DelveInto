#include "FPSPlayer.h"

#include "Entities/Controllers/DefaultPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AFPSPlayer::AFPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 1인칭은 보통 컨트롤러 회전을 Pawn이 그대로 따라가게 둔다
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw   = true;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	// 카메라 컴포넌트 생성 + 캡슐에 붙이기 (1인칭 예시)
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(GetCapsuleComponent());
	FPSCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f)); // 눈 높이 근처로
	FPSCamera->bUsePawnControlRotation = true;
}

void AFPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Player Controller의 OnMove Delegate에 this.HandleMove Method Binding
	if (auto* PC = Cast<ADefaultPlayerController>(GetController()))
	{
		PC->OnMove.AddDynamic(this, &AFPSPlayer::HandleMove);
		PC->OnLook.AddDynamic(this, &AFPSPlayer::HandleLook);
	}
}

void AFPSPlayer::HandleMove(FVector2D DeltaMove)
{
	// Character Controller와 Character Movement Component 유효성 검증
	// FPSPlayer에서 직접적으로 Move Logic을 처리하기 때문에 Character 검증은 하지 않음
	if (!Controller || !GetCharacterMovement())
	{
		return;
	}

	// 값이 0인 경우 무시
	if (DeltaMove.IsNearlyZero())
	{
		return;
	}
	
	const FRotator ControlRotation = GetController()->GetControlRotation();
	const FRotator YawRot(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir,	DeltaMove.Y);
	AddMovementInput(RightDir,		DeltaMove.X);
}

void AFPSPlayer::HandleLook(FVector2D DeltaLook)
{
	UE_LOG(LogTemp, Display, TEXT("Look Axis: %f, %f"), DeltaLook.X, DeltaLook.Y);
	
	if (DeltaLook.IsNearlyZero())
	{
		return;
	}

	AddControllerYawInput(DeltaLook.X);
	AddControllerPitchInput(DeltaLook.Y);
}
