#include "BossHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBossHealthWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (BossHealthBar && MaxHealth > 0.0f)
	{
		// 체력 비율(0.0 ~ 1.0) 계산해서 프로그레스 바에 적용
		float HealthRatio = CurrentHealth / MaxHealth;
		BossHealthBar->SetPercent(HealthRatio);
	}
}

void UBossHealthWidget::SetBossName(const FText& Name)
{
	if (BossNameText)
	{
		BossNameText->SetText(Name);
	}
}