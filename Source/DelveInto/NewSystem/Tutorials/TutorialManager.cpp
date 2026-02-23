#include "TutorialManager.h"
#include "TutorialWidget.h"
#include "Blueprint/UserWidget.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
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

    // 1. 위젯 생성 및 준비
    if (TutorialWidgetClass)
    {
        TutorialUI = CreateWidget<UTutorialWidget>(GetWorld(), TutorialWidgetClass);
        if (TutorialUI) TutorialUI->AddToViewport(100); // 항상 최상단
    }

    if (MainHUDClass)
    {
        MainHUD = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
        if (MainHUD) MainHUD->AddToViewport(0);
    }

    // 2. 1단계 시작 (W키 이동)
    Step1_MoveW();
}

void ATutorialManager::Step1_MoveW()
{
    if (MainHUD) MainHUD->SetVisibility(ESlateVisibility::Hidden);

    if (TutorialUI)
    {
        // [수정] 컷씬으로 넘어가는 연결을 지웁니다! 이제 W를 누르면 시간이 흐르고 자유롭게 걷게 됩니다.
        TutorialUI->OnStepCompleted.Clear(); 
        
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("W 키를 눌러 앞으로 이동하세요.")), EKeys::W);
    }
}

void ATutorialManager::Step2_PlayCutscene()
{
    // 이벤트 연결 해제
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.RemoveDynamic(this, &ATutorialManager::Step2_PlayCutscene);
    }

    // =========================================================
    // [추가] 컷씬 시작 시 플레이어 조작 완벽 차단!
    // =========================================================
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PlayerPawn->DisableInput(PC);
            
            // (선택) 걷던 관성도 즉시 멈추게 하고 싶다면 속도를 0으로 만듭니다.
            if (UCharacterMovementComponent* MovementComp = PlayerPawn->FindComponentByClass<UCharacterMovementComponent>())
            {
                MovementComp->Velocity = FVector::ZeroVector;
            }
        }
    }

    if (IntroSequence)
    {
        ALevelSequenceActor* SequenceActor;
        // [수정] 아래처럼 세팅 객체만 만들고, 문제의 bRestoreState 줄은 지웁니다!
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
    // 1. HUD 복구
    if (MainHUD) MainHUD->SetVisibility(ESlateVisibility::Visible);
    
    // 2. 플레이어 컨트롤러 및 조작 복구
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            // 카메라 시점 복귀
            PC->SetViewTarget(PlayerPawn);
            
            // =========================================================
            // [추가] 컷씬 종료 시 플레이어 조작 다시 활성화!
            // =========================================================
            PlayerPawn->EnableInput(PC);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Cutscene Finished. Camera and Input returned to Player."));
}

// --- 외부(블루프린트나 몬스터 AI)에서 트리거되는 단계들 ---

void ATutorialManager::Step3_AttackTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear();
        // [수정됨] FText::FromString() 사용
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("적이 다가옵니다! [좌클릭]으로 공격하세요.")), EKeys::LeftMouseButton);
    }
}

void ATutorialManager::Step4_DodgeTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear();
        
        // [수정] Shift를 누르면 바로 감시하지 않고, 대기 함수(OnDodgeCompleted)를 부릅니다.
        TutorialUI->OnStepCompleted.AddUniqueDynamic(this, &ATutorialManager::OnDodgeCompleted);
        
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("적의 공격입니다! [Shift]를 눌러 회피하세요.")), EKeys::LeftShift);
    }
}

// [추가된 유예 시간 함수]
void ATutorialManager::OnDodgeCompleted()
{
    TutorialUI->OnStepCompleted.RemoveDynamic(this, &ATutorialManager::OnDodgeCompleted);

    // =========================================================
    // [핵심] 회피 직후 바로 시야를 검사하지 않고, 1.0초의 꿀 같은 여유 시간을 줍니다!
    // (플레이어가 회피 모션을 끝내고 거리를 벌릴 수 있는 시간)
    // =========================================================
    GetWorld()->GetTimerManager().SetTimer(
        TutorialTimerHandle, 
        this, 
        &ATutorialManager::StartLookingCheck, 
        1.0f, // 1초 뒤에 감시 모드로 진입 (필요시 1.5f 등으로 조절)
        false 
    );
}

// (기존과 동일) 이제 진짜 시야 감시를 시작함
void ATutorialManager::StartLookingCheck()
{
    // 0.1초마다 플레이어가 몬스터를 보는지 검사
    GetWorld()->GetTimerManager().SetTimer(
        TutorialTimerHandle, 
        this, 
        &ATutorialManager::CheckPlayerLookingAtMonster, 
        0.1f, 
        true 
    );
}

// 플레이어가 몬스터를 바라보는지 수학적으로 계산하는 함수
void ATutorialManager::CheckPlayerLookingAtMonster()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    
    // 맵에 있는 TargetMonsterClass를 찾습니다.
    AActor* Monster = UGameplayStatics::GetActorOfClass(GetWorld(), TargetMonsterClass);

    if (PC && PC->PlayerCameraManager && Monster)
    {
        // 1. 카메라의 위치와 앞을 바라보는 방향(Forward Vector)
        FVector CameraLoc = PC->PlayerCameraManager->GetCameraLocation();
        FVector CameraForward = PC->PlayerCameraManager->GetCameraRotation().Vector();

        // 2. 플레이어 카메라에서 몬스터를 향하는 선(방향 벡터) 계산
        FVector DirToMonster = (Monster->GetActorLocation() - CameraLoc).GetSafeNormal();

        // 3. 내적(Dot Product) 계산: 완벽히 정면이면 1.0, 90도면 0.0, 뒤통수면 -1.0
        float DotResult = FVector::DotProduct(CameraForward, DirToMonster);

        // 4. 시야각 체크 (0.85 이상이면 대략 화면 중앙 부근에 몬스터가 들어왔다는 뜻입니다)
        if (DotResult > 0.85f)
        {
            // 감시 타이머 끄기 (더 이상 검사 안 함)
            GetWorld()->GetTimerManager().ClearTimer(TutorialTimerHandle);

            // 적을 바라보았으니 0.3초 정도 찰나의 여유를 주고 스킬 창을 띄웁니다!
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
        // 만약 에디터에서 몬스터 할당을 깜빡했거나 몬스터가 죽었다면 타이머 끄고 바로 진행
        GetWorld()->GetTimerManager().ClearTimer(TutorialTimerHandle);
        Step5_SkillTutorial();
    }
}

void ATutorialManager::Step5_SkillTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear();
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("[E]를 눌러 강력한 스킬을 사용하세요!")), EKeys::E);
    }
}

void ATutorialManager::Step6_DoorTutorial()
{
    if (TutorialUI)
    {
        TutorialUI->OnStepCompleted.Clear();
        TutorialUI->OnStepCompleted.AddUniqueDynamic(this, &ATutorialManager::EnterDungeon);
        TutorialUI->ShowTutorialPrompt(FText::FromString(TEXT("전투 승리! [F]를 눌러 던전에 입장하세요.")), EKeys::F);
    }
}

void ATutorialManager::EnterDungeon()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // 1. 화면이 까매지는 동안 움직이거나 스킬을 못 쓰게 조작 차단
        if (APawn* PlayerPawn = PC->GetPawn())
        {
            PlayerPawn->DisableInput(PC);
        }

        // 2. 카메라 페이드 아웃 실행
        // (FromAlpha: 0.0 투명 -> ToAlpha: 1.0 불투명(검은색), Duration: 1.0초, Color: Black, 오디오페이드: false, Hold: true(검은화면 유지))
        if (PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 1.0f, FLinearColor::Black, false, true);
        }
    }

    // 3. 1초(페이드 아웃 시간) 뒤에 실제로 맵을 이동시킵니다!
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