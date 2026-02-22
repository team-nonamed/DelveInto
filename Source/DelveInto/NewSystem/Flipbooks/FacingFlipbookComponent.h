#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "FacingFlipbookComponent.generated.h"

/**
 * 항상 플레이어의 카메라를 향해 바라보는(Billboarding) Flipbook 컴포넌트입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UFacingFlipbookComponent : public UPaperFlipbookComponent
{
	GENERATED_BODY()

public:
	UFacingFlipbookComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Z축(Yaw)으로만 회전할지 여부. 
	// true면 위아래로 쳐다봐도 판때기가 기울지 않고 빳빳하게 서 있습니다. (둠 스타일)
	// false면 카메라가 공중에 있으면 위를 쳐다보며 완전히 눕습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billboarding")
	bool bLockPitchAndRoll = true;

	// Paper2D 스프라이트 원본이 그려진 축에 맞추기 위한 보정 회전값.
	// 보통 언리얼 Paper2D는 X나 Y축이 Normal이므로, 옆면이 보인다면 Yaw에 90이나 -90을 넣으세요.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billboarding")
	FRotator RotationOffset = FRotator(0.0f, 90.0f, 0.0f);
};