// DelveHealthBarWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h" // 프로그레스 바 헤더
#include "DelveHealthBarWidget.generated.h"

UCLASS()
class DELVEINTO_API UDelveHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// [중요] 이름이 일치하는 위젯을 자동으로 바인딩함
	// 블루프린트에서 Progress Bar 이름을 반드시 "HealthBar"로 지어야 함!
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	// 체력 업데이트 함수
	void UpdateHealthRatio(float Ratio);
};