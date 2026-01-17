#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HurtHandler.generated.h"

USTRUCT(BlueprintType)
struct FHurtRequest
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Sender = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> Receiver = nullptr;

	UPROPERTY()
	float Damage = 0.f;
};

USTRUCT(BlueprintType)
struct FHurtResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bSuccess = true;

	UPROPERTY()
	float AppliedDamage = 0.f;
};

UINTERFACE()
class DELVEINTO_API UHurtHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 요청대로: UFUNCTION 없이 “가상함수”만 둡니다.
 * BP는 이 인터페이스를 직접 구현하기보다, Host의 델리게이트 바인딩/연동 정도만 수행하는 용도.
 *
 * 주의:
 * - UFUNCTION 기반 Execute_ 호출을 쓰지 않으므로, 호출자는 반드시 “C++로 구현된 객체”를 대상으로 Cast<IHurtHandler> 해야 합니다.
 * - 가장 안전한 패턴은 “Actor(Host)가 IHurtHandler를 구현하고 내부 HealthHandler 컴포넌트로 위임”입니다.
 */
class DELVEINTO_API IHurtHandler
{
	GENERATED_BODY()

public:
	virtual FHurtResult HandleHurt(const FHurtRequest& Request) = 0;
};
