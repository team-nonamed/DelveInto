#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputTriggers.h"
#include "Engine/DataAsset.h"
#include "Types/SkillTypes.h"
#include "InputSignalConfig.generated.h"

/**
 * “어떤 InputAction + 어떤 TriggerEvent”를 키로 사용하기 위한 구조체.
 * - TMap Key로 쓰려면 operator== + GetTypeHash가 필요.
 */
USTRUCT(BlueprintType)
struct FActionSignal
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UInputAction> Action = nullptr;

	UPROPERTY(EditDefaultsOnly)
	ETriggerEvent TriggerEvent = ETriggerEvent::Started;

	friend bool operator==(const FActionSignal& A, const FActionSignal& B)
	{
		return A.Action == B.Action && A.TriggerEvent == B.TriggerEvent;
	}
};

FORCEINLINE uint32 GetTypeHash(const FActionSignal& S)
{
	const uint32 ActionHash = GetTypeHash(S.Action.Get());
	const uint32 TriggerHash = ::GetTypeHash(static_cast<uint8>(S.TriggerEvent));
	return HashCombine(ActionHash, TriggerHash);
}

/**
 * 입력 → 의미론적 스킬 슬롯(Designator) 매핑.
 * - SkillData는 Designator를 몰라야 하므로, 이 매핑은 Input 계층/Host 계층에 존재.
 */
USTRUCT(BlueprintType)
struct FSkillCommand
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	ESkillSlot Designator = ESkillSlot::BaseAttack;
};

UCLASS(BlueprintType)
class DELVEINTO_API UInputSignalConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FActionSignal, FSkillCommand> SkillSignals;
};
