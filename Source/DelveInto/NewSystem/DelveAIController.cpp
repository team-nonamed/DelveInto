#include "DelveAIController.h"
#include "Kismet/GameplayStatics.h"
#include "DelveEnemy.h"

void ADelveAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
    
	// 플레이어 찾기
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ADelveAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 몬스터가 죽었으면 AI 중지
	if (ADelveEnemy* MyEnemy = Cast<ADelveEnemy>(GetPawn()))
	{
		if (MyEnemy->IsDead()) return; // IsDead() Getter 하나 만들면 좋음 (혹은 bIsDead public 접근)
	}

	// [수정된 부분] 플레이어가 없으면 다시 찾기 시도
	if (PlayerPawn == nullptr)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
	
	// 플레이어가 존재하고, 거리가 가까우면 추적 (예: 15미터 이내)
	if (PlayerPawn)
	{
		float Distance = FVector::Dist(GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());
        
		// --- 디버깅 로그 추가 ---
		// 화면에 매 프레임 거리를 출력합니다. (초록색 글씨)
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("Distance: %f"), Distance));
		// ---------------------

		if (Distance < 1500.0f) 
		{
			MoveToActor(PlayerPawn, 100.0f);
		}
	}
	else
	{
		// 플레이어를 못 찾았다면?
		UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is NULL!"));
	}
}