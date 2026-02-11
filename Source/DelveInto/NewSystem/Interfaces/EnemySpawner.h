#pragma once

#pragma once

#include "CoreMinimal.h"
#include "NewSystem/DelveEnemy.h"
#include "UObject/Interface.h"
#include "EnemySpawner.generated.h"

// 1. 리플렉션용 UInterface (수정하지 않음)
UINTERFACE(MinimalAPI, Blueprintable)
class UEnemySpawner : public UInterface
{
	GENERATED_BODY()
};

// 2. 실제 기능을 담을 IInterface
class DELVEINTO_API IEnemySpawner
{
	GENERATED_BODY()

protected:
#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Debug")
	void UpdatePreviewRenderer();
#endif
	
public:
	// =============================================================
	// 핵심 메서드: Spawn
	// =============================================================
	// BlueprintNativeEvent: C++에서도 구현 가능하고, 블루프린트에서도 오버라이드 가능함
	// BlueprintCallable: 다른 곳에서 이 함수를 호출 가능함
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawn")
	ADelveEnemy* SpawnEnemy();
};