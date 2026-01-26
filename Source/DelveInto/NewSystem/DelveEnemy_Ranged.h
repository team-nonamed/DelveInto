#pragma once

#include "CoreMinimal.h"
#include "DelveEnemy.h"
#include "DelveEnemy_Ranged.generated.h"

class ADelveProjectile;

UCLASS()
class DELVEINTO_API ADelveEnemy_Ranged : public ADelveEnemy
{
	GENERATED_BODY()
    
public:
	ADelveEnemy_Ranged();

protected:
	// [설정] 발사할 투사체 블루프린트 클래스
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<ADelveProjectile> ProjectileClass;

	// [설정] 발사 위치 오프셋 (입, 지팡이 끝 등)
	UPROPERTY(EditAnywhere, Category = "Combat")
	FVector MuzzleOffset;

	// 부모의 근접 공격 함수를 오버라이드하여 투사체 발사로 변경
	virtual void ExecuteAttack() override;
};