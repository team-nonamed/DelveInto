#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHealthWidget.generated.h"

UCLASS()
class DELVEINTO_API UBossHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 체력바 갱신 함수
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	// 보스 이름 세팅 함수
	void SetBossName(const FText& Name);

protected:
	// 블루프린트에서 만들 ProgressBar(체력바)와 연결할 포인터
	// (이름을 반드시 똑같이 지어야 합니다!)
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* BossHealthBar;

	// 블루프린트에서 만들 TextBlock(보스 이름)과 연결할 포인터
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BossNameText;
};