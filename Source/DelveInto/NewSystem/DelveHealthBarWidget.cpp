// DelveHealthBarWidget.cpp

#include "DelveHealthBarWidget.h"

void UDelveHealthBarWidget::UpdateHealthRatio(float Ratio)
{
	if (HealthBar)
	{
		// 0.0 ~ 1.0 사이로 비율 설정
		HealthBar->SetPercent(Ratio);
	}
}