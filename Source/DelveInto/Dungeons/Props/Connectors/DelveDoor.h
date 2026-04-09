#pragma once

#include "CoreMinimal.h"
#include "Dungeons/Props/RoomConnector.h"
#include "DelveDoor.generated.h"

UCLASS()
class DELVEINTO_API ADelveDoor : public ARoomConnector // [수정 2] ARoomConnector 상속
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* DoorFrame;

	// [수정 3] 컴포넌트 부착을 위해 AActor* 에서 UStaticMeshComponent* 로 변경
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* DoorPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UBoxComponent* BlockerCollision;

	// --- 상태 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bIsOpen = false; 

	// ==============================================================
	// [수정 4] 부모의 가상 함수 오버라이드 (기존 UnlockAndOpen / CloseAndLock 대체)
	// ==============================================================
	virtual void OpenConnector(bool bPlaySound = true) override;
	virtual void CloseConnector() override;
    
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* OpenSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* CloseSound;

	// 블루프린트 구현부 (기존 유지)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
	void OpenDoor(bool Sound);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
	void CloseDoor();
};