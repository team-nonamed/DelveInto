#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

class UTutorialWidget;
class ULevelSequence;
class ALevelSequenceActor;
class UUserWidget;

UCLASS()
class DELVEINTO_API ATutorialManager : public AActor
{
	GENERATED_BODY()
    
public:    
	ATutorialManager();

protected:
	virtual void BeginPlay() override;

	// --- 컴포넌트 및 데이터 ---
	UPROPERTY(EditAnywhere, Category = "Tutorial|UI")
	TSubclassOf<UTutorialWidget> TutorialWidgetClass;
    
	UPROPERTY()
	UTutorialWidget* TutorialUI;

	// 메인 HUD (체력바, 무기 등) 레퍼런스
	UPROPERTY(EditAnywhere, Category = "Tutorial|UI")
	TSubclassOf<UUserWidget> MainHUDClass;
    
	UPROPERTY()
	UUserWidget* MainHUD;

	// 시퀀서 데이터 (던전 전경 -> 몬스터 줌인)
	UPROPERTY(EditAnywhere, Category = "Tutorial|Cinematic")
	ULevelSequence* IntroSequence;

	// --- 튜토리얼 단계 함수들 ---
	void Step1_MoveW();
    
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void Step2_PlayCutscene();
    
	UFUNCTION()
	void OnCutsceneFinished();

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void Step5_SkillTutorial(); 

	// [수정] 0.5초 무조건 대기가 아니라, 감시 모드로 진입하는 함수
	UFUNCTION()
	void StartLookingCheck();

	// [추가] 0.1초마다 플레이어가 몬스터를 보는지 검사할 함수
	UFUNCTION()
	void CheckPlayerLookingAtMonster();

	// [추가] 맵에 있는 몬스터를 찾기 위한 클래스 지정 변수
	UPROPERTY(EditAnywhere, Category = "Tutorial|Target")
	TSubclassOf<AActor> TargetMonsterClass;

	// [추가] Shift 창이 닫혔을 때 제일 먼저 불릴 함수 (유예 시간 시작)
	UFUNCTION()
	void OnDodgeCompleted();

	// 튜토리얼을 완전히 끝내고 진짜 던전으로 넘어가는 함수
	UFUNCTION()
	void EnterDungeon();

	// [추가] 페이드 아웃이 끝난 뒤 맵을 이동할 함수
	UFUNCTION()
	void TransitionToNextLevel();

public:
	
	
	// 몬스터나 트리거 박스에서 호출할 공개 함수들
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void Step3_AttackTutorial(); // 몬스터가 다가왔을 때 호출

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void Step4_DodgeTutorial(); // 몬스터가 공격 모션을 취할 때 호출
	
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void Step6_DoorTutorial(); // 몬스터 사망 시 호출

private:
	FTimerHandle TutorialTimerHandle;
};