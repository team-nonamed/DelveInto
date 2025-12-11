#include "Entities/Controllers/DefaultPlayerController.h"
#include "EnhancedInputSubsystems.h"

#pragma region Input Context 관리 Methods

bool ADefaultPlayerController::SetInputContexts(const TArray<EPlayerInputContext>& NewContexts)
{
	ActiveInputContexts.Empty();

	for (EPlayerInputContext Context : NewContexts)
	{
		// 사전에 정의된 MappingContext 에만 한정해서 활성화
		if (InputMappingContexts.Contains(Context))
		{
			ActiveInputContexts.Add(Context);
		}
	}

	RebuildInputMappings();

	// 하나도 활성화되지 않았다면 실패로 간주
	return ActiveInputContexts.Num() > 0;
}

bool ADefaultPlayerController::AddInputContext(EPlayerInputContext Context)
{
	// 정의되지 않은 Context라면 실패
	if (!InputMappingContexts.Contains(Context))
	{
		return false;
	}

	// 이미 활성화된 경우에도 일관성을 위해 true를 반환 (존재 자체는 성공)
	const bool bWasAlreadyActive = ActiveInputContexts.Contains(Context);

	ActiveInputContexts.Add(Context);
	RebuildInputMappings();

	return true;
}

bool ADefaultPlayerController::AddInputContexts(const TArray<EPlayerInputContext>& Contexts)
{
	bool bAnySuccess = false;

	for (EPlayerInputContext Context : Contexts)
	{
		if (InputMappingContexts.Contains(Context))
		{
			ActiveInputContexts.Add(Context);
			bAnySuccess = true;
		}
	}

	RebuildInputMappings();

	return bAnySuccess;
}

bool ADefaultPlayerController::RemoveInputContext(EPlayerInputContext Context)
{
	// 현재 활성화된 Context에 없으면 실패
	if (!ActiveInputContexts.Contains(Context))
	{
		return false;
	}

	ActiveInputContexts.Remove(Context);
	RebuildInputMappings();

	return true;
}

#pragma endregion Input Context 관리 Methods

#pragma region Life Cycle / Initialization

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 현재 ActiveInputContexts 기준으로 한 번 정리
	// (필요하면 BP에서 BeginPlay 전에 ActiveInputContexts를 세팅해 둘 수 있음)
	RebuildInputMappings();
}

void ADefaultPlayerController::RebuildInputMappings()
{
	// Player 유효성 검증
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	// Enhanced Input System 검증
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!Subsystem)
	{
		return;
	}

	// 기존 매핑 제거
	Subsystem->ClearAllMappings();

	// ActiveInputContexts를 기준으로 MappingContext 다시 추가
	int32 Priority = 0;

	for (const EPlayerInputContext Context : ActiveInputContexts)
	{
		if (const TObjectPtr<UInputMappingContext>* Found = InputMappingContexts.Find(Context))
		{
			if (UInputMappingContext* MappingContext = Found->Get())
			{
				Subsystem->AddMappingContext(MappingContext, Priority++);
			}
		}
	}

	// 3) UE 저수준 InputMode 설정 (예시 정책)
	//    - Inventory Context가 하나라도 활성화되어 있으면 Game+UI 모드
	//    - 그 외에는 GameOnly
	const bool bHasInventory = ActiveInputContexts.Contains(EPlayerInputContext::InUI);

	if (bHasInventory)
	{
		FInputModeGameAndUI Mode;
		Mode.SetHideCursorDuringCapture(false);
		SetInputMode(Mode);
		bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
		bShowMouseCursor = false;
	}
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 여기에서 Enhanced Input 기반으로 UInputAction 들을
	// EmitMove / EmitLook / EmitSprintStart / EmitSprintEnd /
	// EmitInteractStart / EmitInteractEnd / EmitOpenInventory 에 바인딩하면 된다.
	//
	// 예시 (실제 구현 시에는 UInputAction* 프로퍼티를 헤더에 선언해 둔 뒤 사용):
	//
	// if (auto* EnhancedComp = Cast<UEnhancedInputComponent>(InputComponent))
	// {
	//     EnhancedComp->BindAction(MoveAction, ETriggerEvent::Triggered, this,
	//         &ADefaultPlayerController::HandleMoveInput);
	//     ...
	// }
}
#pragma endregion

#pragma region Event Broadcast Methods

// =======================
// Emit / 이벤트 브로드캐스트
// =======================

void ADefaultPlayerController::EmitMove(const FVector2D& DeltaMove)
{
	OnMove.Broadcast(DeltaMove);
}

void ADefaultPlayerController::EmitLook(const FVector2D& DeltaLook)
{
	OnLook.Broadcast(DeltaLook);
}

void ADefaultPlayerController::EmitSprintStart()
{
	OnSprintStart.Broadcast();
}

void ADefaultPlayerController::EmitSprintEnd()
{
	OnSprintEnd.Broadcast();
}

void ADefaultPlayerController::EmitInteractStart()
{
	OnInteractStart.Broadcast();
}

void ADefaultPlayerController::EmitInteractEnd()
{
	OnInteractEnd.Broadcast();
}

void ADefaultPlayerController::EmitOpenInventory()
{
	// 인벤토리 Context 활성 여부에 따라 Start/End를 나눠서 쏘는 토글 방식 예시.
	// (실제 인벤토리 열기/닫기는 별도의 Inventory 컴포넌트가 이 이벤트를 받아 처리)
	const bool bIsInventoryActive = ActiveInputContexts.Contains(EPlayerInputContext::InUI);

	if (bIsInventoryActive)
	{
		OnOpenInventoryEnd.Broadcast();
	}
	else
	{
		OnOpenInventoryStart.Broadcast();
	}
}
#pragma endregion Event Broadcast Methods