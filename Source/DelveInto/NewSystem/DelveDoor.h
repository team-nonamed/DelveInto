#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DelveDoor.generated.h"

UCLASS()
class DELVEINTO_API ADelveDoor : public AActor
{
	GENERATED_BODY()
    
public:    
	ADelveDoor();

protected:
	virtual void BeginPlay() override;

public:
	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USceneComponent* RootScene;

	// 문틀 (움직이지 않는 외곽 부분)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* DoorFrame;

	// 문짝 (열리고 닫힐 때 사라지거나 움직이는 부분)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class AActor* DoorPanel;

	// 문이 닫혔을 때 플레이어를 막는 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UBoxComponent* BlockerCollision;

	// --- 상태 및 함수 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bIsOpen = false; // 기본적으로 열려있는지?

	// 문을 닫고 잠금 (전투 시작 시)
	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseAndLock();

	// 문을 열고 통과 가능하게 함 (전투 종료 시)
	UFUNCTION(BlueprintCallable, Category = "Door")
	void UnlockAndOpen(bool Sound);
    
	// (선택) 문이 열릴 때/닫힐 때 재생할 소리
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* OpenSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* CloseSound;

	// [핵심] C++에서는 선언만! 구현은 BP에서!
	// BlueprintCallable을 붙이면 다른 블루프린트에서도 호출 가능합니다.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
	void OpenDoor(bool Sound);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
	void CloseDoor();
};