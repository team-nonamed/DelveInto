#include "InventoryHandler.h"

#include "NewSystem/Items/ItemEffect.h"

// 디버그 로그용 카테고리 (필요하다면 헤더에 DECLARE_LOG_CATEGORY_EXTERN 추가)
DEFINE_LOG_CATEGORY_STATIC(LogInventory, Log, All);

UInventoryHandler::UInventoryHandler()
{
    // 인벤토리는 매 프레임 업데이트할 필요가 없으므로 Tick을 끕니다.
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryHandler::BeginPlay()
{
    Super::BeginPlay();

    // 설정된 MaxSlots 개수만큼 빈 슬롯 배열을 초기화합니다.
    Slots.SetNum(MaxSlots);
}

int32 UInventoryHandler::AddItem(UItemData* InItemData, int32 AmountToAdd)
{
    // 예외 처리: 데이터가 없거나 0개 이하면 무시
    if (!InItemData || AmountToAdd <= 0)
    {
        return AmountToAdd;
    }

    // 1. 필요한 게임플레이 태그를 코드로 요청합니다.
    FGameplayTag GoldCoinId = FGameplayTag::RequestGameplayTag(FName("Item.Id.Coin.Common"));
    FGameplayTag MagicShardId = FGameplayTag::RequestGameplayTag(FName("Item.Id.Shard.Common"));

    // 2. 재화(금화, 마석) 처리: Enum 비교 대신 MatchesTagExact 사용
    if (InItemData->ItemId.MatchesTagExact(GoldCoinId))
    {
        GoldAmount += AmountToAdd;
        UE_LOG(LogInventory, Display, TEXT("금화 획득: %d (총: %d)"), AmountToAdd, GoldAmount);
        
        if (OnInventoryUpdated.IsBound())
        {
            OnInventoryUpdated.Broadcast(InItemData, AmountToAdd, GoldAmount);
        }
        return 0; 
    }
    
    if (InItemData->ItemId.MatchesTagExact(MagicShardId))
    {
        MagicStoneAmount += AmountToAdd;
        UE_LOG(LogInventory, Display, TEXT("마석 획득: %d (총: %d)"), AmountToAdd, MagicStoneAmount);
        
        if (OnInventoryUpdated.IsBound())
        {
            OnInventoryUpdated.Broadcast(InItemData, AmountToAdd, MagicStoneAmount);
        }
        return 0;
    }

    // 2. 일반 아이템 처리: 기존에 같은 아이템이 있는 슬롯을 찾아 한도(MaxStack)까지 채움
    int32 RemainingAmount = AmountToAdd;

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        // 동일한 아이템이 들어있고, 아직 MaxStack에 도달하지 않은 슬롯을 찾음
        if (Slots[i].ItemData == InItemData && Slots[i].Amount < InItemData->MaxStack)
        {
            int32 SpaceLeft = InItemData->MaxStack - Slots[i].Amount;
            
            // 여유 공간에 다 들어가는 경우
            if (RemainingAmount <= SpaceLeft)
            {
                Slots[i].Amount += RemainingAmount;
                RemainingAmount = 0;
                break; // 다 넣었으니 루프 종료
            }
            // 여유 공간이 부족한 경우 (꽉 채우고 다음 슬롯으로)
            else
            {
                Slots[i].Amount = InItemData->MaxStack;
                RemainingAmount -= SpaceLeft;
            }
        }
    }

    // 3. 꽉 찬 슬롯들을 지나고도 아이템이 남았다면, 비어있는 슬롯을 찾아서 새로 할당함
    if (RemainingAmount > 0)
    {
        for (int32 i = 0; i < Slots.Num(); ++i)
        {
            if (Slots[i].IsEmpty())
            {
                Slots[i].ItemData = InItemData;
                
                // 남은 양이 최대 중첩 개수보다 작거나 같으면 전부 넣음
                if (RemainingAmount <= InItemData->MaxStack)
                {
                    Slots[i].Amount = RemainingAmount;
                    RemainingAmount = 0;
                    break; // 다 넣었으니 루프 종료
                }
                // 남은 양이 최대 중첩 개수보다 크면 꽉 채우고 다시 다음 빈 슬롯 탐색
                else
                {
                    Slots[i].Amount = InItemData->MaxStack;
                    RemainingAmount -= InItemData->MaxStack;
                }
            }
        }
    }

    // 결과 처리 및 로그
    int32 AddedAmount = AmountToAdd - RemainingAmount; // 실제로 인벤토리에 들어간 개수

    if (AddedAmount > 0)
    {
        UE_LOG(LogInventory, Display, TEXT("%s %d개 획득!"), *InItemData->ItemName.ToString(), AddedAmount);
        
        if (OnInventoryUpdated.IsBound())
        {
            // (참고: 일반 아이템은 슬롯별로 나뉘어 있으므로 TotalAmount 파라미터에는 들어간 개수만 전달하거나, 별도 합산 로직 필요)
            OnInventoryUpdated.Broadcast(InItemData, AddedAmount, AddedAmount);
        }
    }

    if (RemainingAmount > 0)
    {
        UE_LOG(LogInventory, Warning, TEXT("인벤토리가 꽉 찼습니다! %s %d개를 획득하지 못했습니다."), *InItemData->ItemName.ToString(), RemainingAmount);
    }

    // 획득하지 못하고 남은 개수를 반환 (땅에 남겨두기 위함)
    return RemainingAmount;
}

bool UInventoryHandler::ConsumeItemAtSlot(int32 SlotIndex, int32 ConsumeAmount)
{
    // 유효하지 않은 슬롯 인덱스거나 소비할 개수가 0 이하면 실패
    if (!Slots.IsValidIndex(SlotIndex) || ConsumeAmount <= 0)
    {
        UE_LOG(LogInventory, Error, TEXT("잘못된 슬롯 인덱스거나 소비 수량이 잘못되었습니다."));
        return false;
    }

    FInventorySlot& TargetSlot = Slots[SlotIndex];

    // 슬롯이 비어있거나, 가진 개수보다 많이 소비하려고 하면 실패
    if (TargetSlot.IsEmpty() || TargetSlot.Amount < ConsumeAmount)
    {
        UE_LOG(LogInventory, Warning, TEXT("%d번 슬롯의 아이템이 부족합니다."), SlotIndex);
        return false;
    }

    // 임시로 아이템 정보 저장 (UI 업데이트용)
    UItemData* ConsumedItemData = TargetSlot.ItemData;

    // 아이템 차감
    TargetSlot.Amount -= ConsumeAmount;

    // 만약 개수가 0 이하가 되었다면 슬롯을 완전히 비움 (ItemData = nullptr)
    if (TargetSlot.Amount <= 0)
    {
        TargetSlot.Clear();
    }

    UE_LOG(LogInventory, Display, TEXT("%d번 슬롯에서 %s %d개 소비함. (남은 개수: %d)"), 
           SlotIndex, *ConsumedItemData->ItemName.ToString(), ConsumeAmount, TargetSlot.Amount);

    // [핵심 추가] 이 아이템에 등록된 모든 효과(Effect)를 실행합니다!
    for (UItemEffect* Effect : ConsumedItemData->ItemEffects)
    {
        if (Effect)
        {
            Effect->ApplyEffect(GetOwner());
        }
    }

    if (OnInventoryUpdated.IsBound())
    {
        OnInventoryUpdated.Broadcast(ConsumedItemData, -ConsumeAmount, TargetSlot.Amount);
    }

    return true;
}