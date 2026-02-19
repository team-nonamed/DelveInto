#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h" 
#include "NewSystem/Entities/DelveProjectile.h"
#include "SpriteProjectile.generated.h"

/**
 * 기본 투사체를 상속받아, 항상 카메라를 바라보는 수평 스프라이트(Billboard) 기능을 추가한 클래스
 */
UCLASS()
class DELVEINTO_API ASpriteProjectile : public ADelveProjectile
{
	GENERATED_BODY()

public:
	ASpriteProjectile();

protected:
	virtual void BeginPlay() override;

	// 회전 계산을 위해 Tick을 활성화합니다.
	virtual void Tick(float DeltaTime) override;

public:
	// [신규] 스프라이트를 표시할 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	UPaperSpriteComponent* SpriteComponent;
};