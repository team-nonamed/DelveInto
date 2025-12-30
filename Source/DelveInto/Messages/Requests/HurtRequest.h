#pragma once

USTRUCT(BlueprintType)
struct FHurtRequest
{
	GENERATED_BODY()

	TScriptInterface<class IHurtInitiator> Sender = nullptr;

	TScriptInterface<class IHurtReceiver> Receiver = nullptr;

	float Damage = 0.0f;

	bool bIsCritical = false;

	// 공격 타입은 존재하지 않음
};
