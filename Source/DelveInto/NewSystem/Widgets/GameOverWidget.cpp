#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼을 마우스로 직접 클릭했을 때도 작동하게 유지
	if (RetryButton)
	{
		RetryButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRetryClicked);
	}
}

// [신규] 키보드 입력 처리
FReply UGameOverWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// 누른 키가 '스페이스바(SpaceBar)' 라면?
	if (InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		OnRetryClicked(); // 재시작 함수 즉시 실행
        
		// 입력을 처리했음을 엔진에 알림
		return FReply::Handled(); 
	}

	// 스페이스바가 아니면 원래 하던 대로 처리
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UGameOverWidget::OnRetryClicked()
{
	// 현재 맵 이름 가져와서 다시 열기 (재시작)
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
}