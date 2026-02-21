#include "Heal.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Entities/Characters/Handlers/HealthHandler.h" // 체력 핸들러

void UItemEffect_Heal::ApplyEffect_Implementation(AActor* Target)
{
	if (ADelveCharacter* Character = Cast<ADelveCharacter>(Target))
	{
		if (UHealthHandler* HealthHandler = Character->FindComponentByClass<UHealthHandler>())
		{
			// 체력을 회복시키는 로직 (HealthHandler에 Heal 함수가 있다고 가정)
			// HealthHandler->Heal(HealAmount); 
            
			// 만약 Heal 함수가 아직 없다면 직접 더해줍니다:
			HealthHandler->CurrentHealth = FMath::Clamp(HealthHandler->CurrentHealth + HealAmount, 0.0f, HealthHandler->MaxHealth);
            
			UE_LOG(LogTemp, Display, TEXT("%s가 체력을 %f만큼 회복했습니다!"), *Target->GetName(), HealAmount);
		}
	}
}