#include "SkillSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USkillSlotWidget::BindSkill(USkillBase* InSkill)
{
   TargetSkill = InSkill;
    
   // 1. [초기 상태 갱신] 장착된 스킬이 없다면?
   if (!InSkill) 
   {
      if (IconImage)
      {
         // 아이콘을 투명하게 숨겨서 빈칸으로 만듭니다.
         IconImage->SetVisibility(ESlateVisibility::Hidden); 
      }
      if (CooldownText)
      {
         CooldownText->SetVisibility(ESlateVisibility::Hidden);
      }
      return;
   }

   // 2. 장착된 스킬이 존재한다면 아이콘을 다시 보이게 합니다.
   if (IconImage)
   {
      IconImage->SetVisibility(ESlateVisibility::Visible);
        
      CooldownDynamicMaterial = IconImage->GetDynamicMaterial();
       
      if (CooldownDynamicMaterial && InSkill->SkillIcon)
      {
         CooldownDynamicMaterial->SetTextureParameterValue(FName("Texture"), InSkill->SkillIcon);
         CooldownDynamicMaterial->SetScalarParameterValue(FName("Percent"), 0.0f); 
      }
      else if (InSkill->SkillIcon)
      {
         IconImage->SetBrushFromTexture(InSkill->SkillIcon);
      }
   }
}

void USkillSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!TargetSkill.IsValid()) return;

    float Remaining = TargetSkill->CooldownSeconds;
    float MaxCD = TargetSkill->MaxCooldownSeconds;

    if (Remaining > 0.0f)
    {
       // 쿨타임 중: 숫자 텍스트 표시
       if (CooldownText->GetVisibility() != ESlateVisibility::Visible)
       {
           CooldownText->SetVisibility(ESlateVisibility::Visible);
       }
       CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(Remaining)));

       // 쿨타임 중: 머티리얼 퍼센트 조절 (1.0 = 방금 씀, 0.0 = 쿨타임 끝남)
       if (CooldownDynamicMaterial && MaxCD > 0.0f)
       {
          float Alpha = Remaining / MaxCD; 
          CooldownDynamicMaterial->SetScalarParameterValue(FName("Percent"), Alpha);
       }
    }
    else
    {
       // 쿨타임 끝남: 숫자 숨기고 퍼센트 0으로 초기화
       if (CooldownText->GetVisibility() != ESlateVisibility::Hidden)
       {
           CooldownText->SetVisibility(ESlateVisibility::Hidden);
       }
       
       if (CooldownDynamicMaterial)
       {
          CooldownDynamicMaterial->SetScalarParameterValue(FName("Percent"), 0.0f);
       }
    }
}