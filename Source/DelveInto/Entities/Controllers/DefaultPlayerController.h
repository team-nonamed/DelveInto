#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMove, FVector2D, MoveAxis);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLook, FVector2D, LookAxis);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenInventoryStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenInventoryEnd);

//TODO: 삭제 및 마이그레이션
UENUM(BlueprintType)
enum class EPlayerInputContext : uint8
{
	FPS,
	Battle,
	InvokeUI,
	InUI,
};

/**
 * 캐릭터의 입력을 처리하는 Class
 */
UCLASS()
class DELVEINTO_API ADefaultPlayerController: public APlayerController
{
	GENERATED_BODY()

#pragma region Properties

//TODO: Action들의 Binding을 Blueprint에서 하거나 혹은 등록용 메서드를 따로 만들어 관리해야 함
public:
#pragma region Actions

	UPROPERTY(BlueprintAssignable, Category="Behaviour Events")
	FOnMove OnMove;

	UPROPERTY(BlueprintAssignable, Category="Behaviour Events")
	FOnLook OnLook;
	
	/**
	 * Binding하리라 기대하는 달리기 시작 Event
	 */
	UPROPERTY(BlueprintAssignable, Category = "Behaviour Events")
	FOnSprintStart OnSprintStart;

	/**
	 * Binding하리라 기대하는 달리기 종료 Event
	 */
	UPROPERTY(BlueprintAssignable, Category = "Behaviour Events")
	FOnSprintEnd OnSprintEnd;

	/**
	 * Binding하리라 기대하는 Interaction 시작 Event
	 */
	UPROPERTY(BlueprintAssignable, Category="Behaviour Events")
	FOnInteractStart OnInteractStart;

	/**
	 * Binding하리라 기대하는 Interaction 종료 Event
	 */
	UPROPERTY(BlueprintAssignable, Category="Behaviour Events")
	FOnInteractEnd OnInteractEnd;

	/**
	 * Binding하리라 기대하는 Inventory 열기 시작 Event
	 */
	UPROPERTY(BlueprintAssignable, Category="Behaviour Events")
	FOnOpenInventoryStart OnOpenInventoryStart;

	/**
	 * Binding하리라 기대하는 Inventory 열기 종료 Event
	 */
	UPROPERTY(BlueprintAssignable, Category="Behaviour Events")
	FOnOpenInventoryEnd OnOpenInventoryEnd;
	
#pragma endregion Actions
	
protected:
#pragma region Input Context
	/**
	 * 미리 사전 정의된 Input Mapping Context들
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Mapping")
	TMap<EPlayerInputContext, TObjectPtr<UInputMappingContext>> InputMappingContexts;

	/**
	 * 현재 활성화된 Input Mapping Context들
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Mapping")
	TSet<EPlayerInputContext> ActiveInputContexts;
#pragma endregion Input Context
	
#pragma endregion Properties

#pragma region Methods

public:
	/**
	 * NewContexts로 전체 Context를 갈아끼우는 Methods
	 * @param NewContexts 갈아끼울 Context
	 * @return 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category="Input Context")
	bool SetInputContexts(const TArray<EPlayerInputContext>& NewContexts);

	/**
	 * Context를 하나 추가하는 Method
	 * @param Context 추가할 Context
	 * @return 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category="Input Context")
	bool AddInputContext(EPlayerInputContext Context);

	/**
	 * Context들을 추가하는 Method
	 * @param Contexts 추가할 Context들
	 * @return 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category="Input Context")
	bool AddInputContexts(const TArray<EPlayerInputContext>& Contexts);

	/**
	 * 선택한 Context를 제거하는 Method
	 * @param Context 제거할 Context
	 * @return 성공 여부 (현재 Context에 제거할 Context가 없을 경우 실패함)
	 */
	UFUNCTION(BlueprintCallable, Category="Input Context")
	bool RemoveInputContext(EPlayerInputContext Context);

protected:
	/**
	 *  게임이 시작할 때 호출되는 Method
	 */
	virtual void BeginPlay() override;
	
	/**
	 * ActiveInputContexts를 기준으로 Enhanced Input을 다시 구성하는 Helper Method
	 */
	void RebuildInputMappings();

	/**
	 * Input Component를 처음에 설정하는 Method
	 */
	virtual void SetupInputComponent() override;

	
#pragma endregion Methods
	
#pragma region Handlers
	
protected:
	/**
	 * Delta 값을 받아서 OnMove Delegate를 Broadcast하는 Method
	 * @param DeltaMove Move Controller 입력 장치의 Delta 값
	 */
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitMove(const FVector2D& DeltaMove);

	/**
	 * Delta 값을 받아서 OnLook Delegate를 Broadcast하는 Method
	 * @param DeltaLook Pointer 입력 장치의 Delta 값
	 */
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitLook(const FVector2D& DeltaLook);

	/**
	 * OnSprintStart Delegate를 Broadcast하는 Method
	 */
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitSprintStart();

	/**
	 * OnSprintEnd Delegate를 Broadcast하는 Method
	 */
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitSprintEnd();

	/**
	 * OnInteractionStart Delegate를 Broadcast하는 Method
	 */
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitInteractStart();

	/**
	 * OnInteractionEnd Delegate를 Broadcast하는 Method
	 */
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitInteractEnd();

	/**
	 * OnOpenInventory Delegate를 Broadcast하는 Method
	 */
	UFUNCTION(BlueprintCallable, Category="Behaviour Events")
	void EmitOpenInventory();
};
