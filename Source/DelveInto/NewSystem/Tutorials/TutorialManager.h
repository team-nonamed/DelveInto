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

    UPROPERTY(EditAnywhere, Category = "Tutorial|UI")
    TSubclassOf<UUserWidget> MainHUDClass;
    
    UPROPERTY()
    UUserWidget* MainHUD;

    // 시퀀서 데이터
    UPROPERTY(EditAnywhere, Category = "Tutorial|Cinematic")
    ULevelSequence* IntroSequence;

    // =========================================================
    // [신규] 몬스터 처치 시 재생될 승리 컷씬 (아웃트로 시퀀스)
    // =========================================================
    UPROPERTY(EditAnywhere, Category = "Tutorial|Cinematic")
    ULevelSequence* OutroSequence;

    // --- 튜토리얼 단계 함수들 ---
    void Step1_MoveW();
    
    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void Step2_PlayCutscene();
    
    UFUNCTION()
    void OnCutsceneFinished();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void Step5_SkillTutorial(); 

    UFUNCTION()
    void StartLookingCheck();

    UFUNCTION()
    void CheckPlayerLookingAtMonster();

    UPROPERTY(EditAnywhere, Category = "Tutorial|Target")
    TSubclassOf<AActor> TargetMonsterClass;

    UFUNCTION()
    void OnDodgeCompleted();

    // =========================================================
    // [신규] 몬스터 사망 시 컷씬 재생 -> 종료 -> 정보 팝업
    // =========================================================
    UFUNCTION()
    void OnOutroCutsceneFinished();

    UFUNCTION()
    void ShowExtraInfoTutorial();

    UFUNCTION()
    void EnterDungeon();

    UFUNCTION()
    void TransitionToNextLevel();

public:
    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void Step3_AttackTutorial(); 

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void Step4_DodgeTutorial(); 
    
    // =========================================================
    // [신규] 몬스터 사망 시 이제 이 함수를 호출해주세요!
    // =========================================================
    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void TriggerMonsterDeathCutscene();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void Step6_DoorTutorial(); 

private:
    FTimerHandle TutorialTimerHandle;
};