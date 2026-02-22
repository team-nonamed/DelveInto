#include "PerkChoiceWidget.h"
#include "NewSystem/Enhances/PerkBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h" // [수정] TimerManager 대신 애니메이션 헤더 사용

void UPerkChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UPerkChoiceWidget::HandleButtonClicked);
	}
}

void UPerkChoiceWidget::SetupChoice(UPerkBase* InPerk, int32 InNextLevel)
{
	if (!InPerk) return;

	CachedPerk = InPerk;
	CachedLevel = InNextLevel;

	// 1. 공통 텍스트 세팅
	if (NameText) NameText->SetText(CachedPerk->PerkName);
	if (DescriptionText) DescriptionText->SetText(CachedPerk->PerkDescription);

	// 2. 레벨별 데이터 세팅
	FPerkLevelData LevelData;
	if (CachedPerk->GetLevelData(CachedLevel, LevelData))
	{
		if (LevelDescriptionText) LevelDescriptionText->SetText(LevelData.LevelDescription);
		if (IconImage && LevelData.LevelIcon) IconImage->SetBrushFromTexture(LevelData.LevelIcon);
	}

	// 3. 부모(SelectionWidget)가 0.15초마다 하나씩 SetupChoice를 호출할 것이므로,
	// 여기서는 호출 즉시 애니메이션을 재생하기만 하면 됩니다.
	if (FadeIn)
	{
		PlayAnimation(FadeIn);
	}
}

void UPerkChoiceWidget::HandleButtonClicked()
{
	if (CachedPerk) OnPerkSelected.Broadcast(CachedPerk, CachedLevel);
}