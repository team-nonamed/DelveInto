#include "TutorialManager.h"
#include "TutorialWidget.h"
#include "Blueprint/UserWidget.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ATutorialManager::ATutorialManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATutorialManager::BeginPlay()
{
    Super::BeginPlay();

    if (TutorialWidgetClass)
    {
        TutorialUI = CreateWidget<UTutorialWidget>(GetWorld(), TutorialWidgetClass);
        if (TutorialUI) TutorialUI->AddToViewport(100); 
    }

    if (MainHUDClass)
    {
        MainHUD = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
        if (MainHUD) MainHUD->AddToViewport(0);
    }

    Step1_MoveW();
}

void ATutorialManager::Step1_MoveW()
{
    if (MainHUD) MainHUD->SetVisibility(ESlateVisibility::Hidden);

    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear(); 
        // 1단계: 마우스 회전(시야) 정도만 허용하고 걷는 건 오직 W만 허용합니다.
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("W 키를 눌러 앞으로 이동하세요.")), EKeys::W, TArray<FKey>());
    }
}

void ATutorialManager::Step2_PlayCutscene()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.RemoveDynamic(this, &ATutorialManager::Step2_PlayCutscene);
    }

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PlayerPawn->DisableInput(PC);
            
            if (UCharacterMovementComponent* MovementComp = PlayerPawn->FindComponentByClass<UCharacterMovementComponent>())
            {
                MovementComp->Velocity = FVector::ZeroVector;
            }
        }
    }

    if (IntroSequence)
    {
        ALevelSequenceActor* SequenceActor;
        FMovieSceneSequencePlaybackSettings Settings;

        ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(), IntroSequence, Settings, SequenceActor);

        if (SequencePlayer)
        {
            SequencePlayer->OnFinished.AddDynamic(this, &ATutorialManager::OnCutsceneFinished);
            SequencePlayer->Play();
        }
    }
    else
    {
        OnCutsceneFinished();
    }
}

void ATutorialManager::OnCutsceneFinished()
{
    if (MainHUD) MainHUD->SetVisibility(ESlateVisibility::Visible);
    
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PC->SetViewTarget(PlayerPawn);
            PlayerPawn->EnableInput(PC);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Intro Cutscene Finished."));
}

void ATutorialManager::Step3_AttackTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear();

        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("적이 다가옵니다! [좌클릭]으로 공격하세요.")), EKeys::LeftMouseButton, TArray<FKey>());
    }
}

void ATutorialManager::Step4_DodgeTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear();
        TutorialUI->OnStepCompleted.AddUniqueDynamic(this, &ATutorialManager::OnDodgeCompleted);
        
        TArray<FKey> AllowedMoveKeys = { EKeys::W, EKeys::A, EKeys::S, EKeys::D };
        
        
        TutorialUI->ShowTutorialPrompt(
            FText::FromString(TEXT("적의 공격입니다! [Shift]와 방향 키를 눌러 회피하세요.")), 
            EKeys::LeftShift, 
            AllowedMoveKeys, 
            true
        );
    }
}

void ATutorialManager::OnDodgeCompleted()
{
    TutorialUI->OnStepCompleted.RemoveDynamic(this, &ATutorialManager::OnDodgeCompleted);

    GetWorld()->GetTimerManager().SetTimer(
        TutorialTimerHandle, 
        this, 
        &ATutorialManager::StartLookingCheck, 
        1.0f, 
        false 
    );
}

void ATutorialManager::StartLookingCheck()
{
    GetWorld()->GetTimerManager().SetTimer(
        TutorialTimerHandle, 
        this, 
        &ATutorialManager::CheckPlayerLookingAtMonster, 
        0.1f, 
        true 
    );
}

void ATutorialManager::CheckPlayerLookingAtMonster()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    AActor* Monster = UGameplayStatics::GetActorOfClass(GetWorld(), TargetMonsterClass);

    if (PC && PC->PlayerCameraManager && Monster)
    {
        FVector CameraLoc = PC->PlayerCameraManager->GetCameraLocation();
        FVector CameraForward = PC->PlayerCameraManager->GetCameraRotation().Vector();
        FVector DirToMonster = (Monster->GetActorLocation() - CameraLoc).GetSafeNormal();

        float DotResult = FVector::DotProduct(CameraForward, DirToMonster);

        if (DotResult > 0.85f)
        {
            GetWorld()->GetTimerManager().ClearTimer(TutorialTimerHandle);

            GetWorld()->GetTimerManager().SetTimer(
                TutorialTimerHandle, 
                this, 
                &ATutorialManager::Step5_SkillTutorial, 
                0.3f, 
                false
            );
        }
    }
    else if (!Monster)
    {
        GetWorld()->GetTimerManager().ClearTimer(TutorialTimerHandle);
        Step5_SkillTutorial();
    }
}

void ATutorialManager::Step5_SkillTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear();
        // 5단계: 스킬(E)을 기다리며, 방향키 조작은 허용합니다.
        TArray<FKey> AllowedMoveKeys = { EKeys::W, EKeys::A, EKeys::S, EKeys::D };
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("[E]를 눌러 강력한 스킬을 사용하세요!")), EKeys::E, AllowedMoveKeys);
    }
}

void ATutorialManager::TriggerMonsterDeathCutscene()
{
    if (MainHUD) MainHUD->SetVisibility(ESlateVisibility::Hidden); 

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PlayerPawn->DisableInput(PC);
            if (UCharacterMovementComponent* MovementComp = PlayerPawn->FindComponentByClass<UCharacterMovementComponent>())
            {
                MovementComp->Velocity = FVector::ZeroVector;
            }
        }
    }

    if (OutroSequence)
    {
        ALevelSequenceActor* SequenceActor;
        FMovieSceneSequencePlaybackSettings Settings;

        ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(), OutroSequence, Settings, SequenceActor);

        if (SequencePlayer)
        {
            SequencePlayer->OnFinished.AddDynamic(this, &ATutorialManager::OnOutroCutsceneFinished);
            SequencePlayer->Play();
        }
    }
    else
    {
        OnOutroCutsceneFinished();
    }
}

void ATutorialManager::OnOutroCutsceneFinished()
{
    if (MainHUD) MainHUD->SetVisibility(ESlateVisibility::Visible);
    
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PC->SetViewTarget(PlayerPawn);
            PlayerPawn->EnableInput(PC);
        }
    }

    ShowExtraInfoTutorial();
}

void ATutorialManager::ShowExtraInfoTutorial()
{
    if (TutorialUI)
    {
        // [복구] 주석 처리되어 있던 코드를 해제했습니다. (안 그러면 스페이스바 눌러도 진행이 안 됩니다!)
        // TutorialUI->OnStepCompleted.Clear();
        // TutorialUI->OnStepCompleted.AddUniqueDynamic(this, &ATutorialManager::Step6_DoorTutorial);
        
        // 정보 창: 스페이스바 대기, 조작은 일단 막아둡니다.
        TutorialUI->ShowTutorialPrompt(
            FText::FromString(TEXT("숫자 패드를 통해 등록된 아이템을, 우클릭과 Q를 통해 특별한 스킬을 사용할 수 있습니다!\n\n[SpaceBar] 를 눌러 계속 진행하세요.")), 
            EKeys::SpaceBar, 
            TArray<FKey>() 
        );
    }
}

void ATutorialManager::Step6_DoorTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.RemoveDynamic(this, &ATutorialManager::Step6_DoorTutorial);
        TutorialUI->OnStepCompleted.AddUniqueDynamic(this, &ATutorialManager::EnterDungeon);
        
        // 마지막 단계: 상호작용(F) 대기, 방향키 이동 허용
        TArray<FKey> AllowedMoveKeys = { EKeys::W, EKeys::A, EKeys::S, EKeys::D };
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("문과 같은 물체는 [F]를 통해 상호작용 할 수 있습니다.")), EKeys::F, AllowedMoveKeys);
    }
}

void ATutorialManager::EnterDungeon()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PlayerPawn->DisableInput(PC);
        }

        if (PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 1.0f, FLinearColor::Black, false, true);
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        TutorialTimerHandle, 
        this, 
        &ATutorialManager::TransitionToNextLevel, 
        1.0f, 
        false
    );
}

void ATutorialManager::TransitionToNextLevel()
{
    UE_LOG(LogTemp, Warning, TEXT("페이드 아웃 완료. LV_DemoDungeon으로 이동합니다."));
    UGameplayStatics::OpenLevel(this, FName("LV_DemoDungeon"));
}