#include "PerkDBSubsystem.h"
#include "NewSystem/Enhances/PerkBase.h"
#include "NewSystem/Enhances/PerkSettings.h"
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"
#include "Engine/AssetManager.h"

void UPerkDBSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadAllPerks();
}

void UPerkDBSubsystem::LoadAllPerks()
{
    const UPerkSettings* Settings = GetDefault<UPerkSettings>();
    if (!Settings) return;

    for (const TSoftObjectPtr<UPerkBase>& SoftPerk : Settings->RegisteredPerks)
    {
        // 당장 동기 로드(LoadSynchronous)를 사용하지만, 로딩 스크린이 있다면 비동기로드를 권장합니다.
        if (UPerkBase* LoadedPerk = SoftPerk.LoadSynchronous())
        {
            CachedPerks.Add(LoadedPerk);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("PerkDB 로드 완료: 총 %d개의 퍽 등록됨."), CachedPerks.Num());
}

UPerkBase* UPerkDBSubsystem::GetPerkById(FGameplayTag PerkId) const
{
    for (UPerkBase* Perk : CachedPerks)
    {
        // PerkBase에 설정된 고유 태그와 비교
        if (Perk && Perk->PerkId == PerkId)
        {
            return Perk;
        }
    }
    return nullptr;
}

TArray<FPerkChoiceData> UPerkDBSubsystem::GetRandomPerksForLevelUp(int32 Count, UPerkHandler* PlayerPerkHandler)
{
    TArray<FPerkChoiceData> Result;
    if (CachedPerks.IsEmpty() || Count <= 0) return Result;

    // 1. 뽑기 가능한 후보군(Pool) 필터링
    TArray<FPerkChoiceData> AvailablePool;
    
    for (UPerkBase* Perk : CachedPerks)
    {
        if (!Perk) continue;

        // 플레이어가 이미 보유 중인지 확인
        int32 CurrentLevel = PlayerPerkHandler ? PlayerPerkHandler->GetPerkLevel(Perk) : 0;

        // 만렙(MaxLevel)에 도달하지 않은 퍽만 풀에 추가
        if (CurrentLevel < Perk->MaxLevel)
        {
            FPerkChoiceData ChoiceData;
            ChoiceData.Perk = Perk;
            ChoiceData.NextLevel = CurrentLevel + 1; // [핵심] 여기서 미리 다음 레벨을 계산해서 넣습니다!
            
            AvailablePool.Add(ChoiceData);
        }
    }

    // 2. 랜덤 셔플 및 추출 (피셔-예이츠 셔플)
    int32 PoolSize = AvailablePool.Num();
    for (int32 i = 0; i < PoolSize - 1; i++)
    {
        int32 SwapIndex = FMath::RandRange(i, PoolSize - 1);
        AvailablePool.Swap(i, SwapIndex);
    }

    // 3. 요청한 개수만큼 반환
    int32 ExtractCount = FMath::Min(Count, PoolSize);
    for (int32 i = 0; i < ExtractCount; i++)
    {
        Result.Add(AvailablePool[i]);
    }

    return Result;
}