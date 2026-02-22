#include "ItemDBSubsystem.h"
#include "Engine/DataTable.h"

void UItemDBSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// [핵심 수정] 생성자가 아니므로 LoadObject를 사용하여 데이터 테이블을 동적으로 불러옵니다!
	// 경로(TEXT)는 본인의 실제 데이터 테이블 경로로 꼭 맞춰주세요.
	UDataTable* ItemTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/NewSystems/Data/Items/DT_ItemDB.DT_ItemDB"));
    
	if (ItemTable)
	{
		TArray<FItemDBRow*> AllRows;
		ItemTable->GetAllRows<FItemDBRow>(TEXT("ItemDBSubsystem"), AllRows);

		for (FItemDBRow* Row : AllRows)
		{
			// 태그가 비어있지 않고 유효한지 검사
			if (Row && Row->ItemID.IsValid())
			{
				ItemDatabase.Add(Row->ItemID, *Row);
			}
		}
        
		UE_LOG(LogTemp, Display, TEXT("[ItemDB] 내부 DB 구축 완료! 총 %d개의 아이템이 로드되었습니다."), ItemDatabase.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemDB] 데이터 테이블 로드 실패! 경로를 확인해주세요."));
	}
}

bool UItemDBSubsystem::GetItemInfo(FGameplayTag ItemID, FItemDBRow& OutItemInfo) const
{
	// 캐싱된 Map에서 ItemID로 초고속 검색
	if (const FItemDBRow* FoundRow = ItemDatabase.Find(ItemID))
	{
		OutItemInfo = *FoundRow;
		return true;
	}
    
	return false;
}