#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "FacingSpriteComponent.generated.h"

UCLASS(classGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELVEINTO_API UFacingSpriteComponent : public UPaperSpriteComponent
{
	GENERATED_BODY()

public:
	UFacingSpriteComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 매 프레임 실행되어 카메라를 바라보게 합니다.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void UpdateFacingRotation();
};