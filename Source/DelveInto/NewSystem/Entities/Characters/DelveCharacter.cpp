#include "DelveCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h" 
#include "Blueprint/UserWidget.h"   
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "NewSystem/Widgets/HandDisplayWidget.h"
#include "NewSystem/Widgets/HealthBarWidget.h"
#include "NewSystem/Entities/Characters/Handlers/CombatHandler.h"
#include "NewSystem/Entities/Characters/Handlers/HealthHandler.h"
#include "NewSystem/Entities/Characters/Handlers/InventoryHandler.h" 
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"
#include "NewSystem/Entities/Enemies/DelveBoss.h"
#include "NewSystem/Interfaces/Interactable.h"
#include "NewSystem/Widgets/Perks/PerkSelectionWidget.h"
#include "UObject/UObjectIterator.h"

ADelveCharacter::ADelveCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    CombatHandler = CreateDefaultSubobject<UCombatHandler>(TEXT("CombatHandler"));
    HealthHandler = CreateDefaultSubobject<UHealthHandler>(TEXT("HealthHandler"));
    InventoryHandler = CreateDefaultSubobject<UInventoryHandler>(TEXT("InventoryHandler"));
    PerkHandler = CreateDefaultSubobject<UPerkHandler>(TEXT("PerkHandler"));
}

void ADelveCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GetWorld()->GetMapName().Contains("DemoDungeon"))
    {
        InventoryHandler->AddItem(PotionData, 3);
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        // 재시작 시 조작 권한을 게임으로 완벽하게 되돌려줍니다.
        FInputModeGameOnly GameOnlyMode;
        PlayerController->SetInputMode(GameOnlyMode);
        PlayerController->bShowMouseCursor = false;
        EnableInput(PlayerController); 

        // Enhanced Input 맵핑
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
        
        // 카메라 페이드 인
        if (PlayerController->PlayerCameraManager)
        {
            PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 1.0f, FLinearColor::Black, false, false);
        }
    }

    if (HealthHandler)
    {
        HealthHandler->OnDamaged.AddDynamic(this, &ADelveCharacter::HandleDamaged);
        HealthHandler->OnDeath.AddDynamic(this, &ADelveCharacter::HandleDeath);
    }

    // UI 생성 
    if (WeaponWidgetClass)
    {
        WeaponWidgetInstance = CreateWidget<UHandDisplayWidget>(GetWorld(), WeaponWidgetClass);
        if (WeaponWidgetInstance)
        {
            WeaponWidgetInstance->AddToViewport();
            WeaponWidgetInstance->InitializeUI(InventoryHandler, CombatHandler);
            
            if (HealthHandler && WeaponWidgetInstance->HealthBar)
            {
                WeaponWidgetInstance->HealthBar->UpdateHealthRatio(HealthHandler->CurrentHealth / HealthHandler->MaxHealth);
            }
            
            CombatHandler->Initialize(WeaponWidgetInstance);
        }
    }

    // 스탯 연동 로직
    if (GetCharacterMovement())
    {
        BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
    }

    if (PerkHandler)
    {
        PerkHandler->OnStatChanged.AddDynamic(this, &ADelveCharacter::HandleStatChanged);
    }
}

void ADelveCharacter::HandleStatChanged(EStatCategory StatType, float DeltaValue)
{
    if (StatType == EStatCategory::MovementSpeed)
    {
        CurrentMoveSpeedMultiplier += DeltaValue;

        if (GetCharacterMovement())
        {
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * CurrentMoveSpeedMultiplier;
            UE_LOG(LogTemp, Warning, TEXT(">> [캐릭터 스탯 적용] 이동 속도 배율: %f | 최종 속도: %f"), CurrentMoveSpeedMultiplier, GetCharacterMovement()->MaxWalkSpeed);
        }
    }
}

void ADelveCharacter::TriggerLevelUp()
{
    if (!PerkSelectionWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PerkSelectionWidgetClass가 설정되지 않았습니다! (캐릭터 블루프린트 확인)"));
        return;
    }

    if (PerkSound)
    {
        UGameplayStatics::PlaySound2D(this, PerkSound);
    }
    
    UPerkSelectionWidget* SelectionUI = CreateWidget<UPerkSelectionWidget>(GetWorld(), PerkSelectionWidgetClass);
    if (SelectionUI && !BossTriggered)
    {
        SelectionUI->AddToViewport();
        SelectionUI->ShowChoices(PerkHandler, 3);
    }
}

void ADelveCharacter::DebugLevelUp()
{
    TriggerLevelUp();
}

void ADelveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADelveCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADelveCharacter::Look);
        
        EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_PrimaryPressed);
        EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &ADelveCharacter::Input_PrimaryReleased);

        EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_SecondaryPressed);
        EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Completed, this, &ADelveCharacter::Input_SecondaryReleased);

        EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_SkillQPressed);
        EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Completed, this, &ADelveCharacter::Input_SkillQReleased);

        EnhancedInputComponent->BindAction(SkillEAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_SkillEPressed);
        EnhancedInputComponent->BindAction(SkillEAction, ETriggerEvent::Completed, this, &ADelveCharacter::Input_SkillEReleased);

        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_JumpPressed);

        EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_DashPressed);
        EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ADelveCharacter::Input_DashReleased);

        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_InteractPressed);

        EnhancedInputComponent->BindAction(OneAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_PotionPressed);
        
        EnhancedInputComponent->BindAction(MapAction, ETriggerEvent::Started, this, &ADelveCharacter::Input_MapPressed);
    }
}

void ADelveCharacter::Input_PrimaryPressed()   { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Primary, true); }
void ADelveCharacter::Input_PrimaryReleased()  { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Primary, false); }
void ADelveCharacter::Input_SecondaryPressed() { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Secondary, true); }
void ADelveCharacter::Input_SecondaryReleased(){ if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Secondary, false); }
void ADelveCharacter::Input_SkillQPressed()    { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillQ, true); }
void ADelveCharacter::Input_SkillQReleased()   { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillQ, false); }
void ADelveCharacter::Input_SkillEPressed()    { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillE, true); }
void ADelveCharacter::Input_SkillEReleased()   { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillE, false); }
void ADelveCharacter::Input_JumpPressed()      { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Jump, true); }
void ADelveCharacter::Input_DashPressed()      { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Dash, true); }
void ADelveCharacter::Input_DashReleased()     { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Dash, false); }

void ADelveCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void ADelveCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void ADelveCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HealthHandler)
    {
        if (!HealthHandler->bIsDead)
        {
            FString HealthMsg = FString::Printf(TEXT("[Player HP] : %.1f / %.1f"), HealthHandler->CurrentHealth, HealthHandler->MaxHealth);
            GEngine->AddOnScreenDebugMessage(123, 0.0f, FColor::Green, HealthMsg);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(123, 0.0f, FColor::Red, TEXT("[Player Dead]"));
        }
    }
}

float ADelveCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (HealthHandler)
    {
        ActualDamage = HealthHandler->ApplyDamage(ActualDamage);
    }

    return ActualDamage;
}

void ADelveCharacter::HandleDamaged(float InMaxHealth, float InCurrentHealth)
{
    // 1. 체력바 UI 업데이트
    if (WeaponWidgetInstance && WeaponWidgetInstance->HealthBar)
    {
        WeaponWidgetInstance->HealthBar->UpdateHealthRatio(InCurrentHealth / InMaxHealth);
    }

    if (HealthHandler && HealthHandler->bIsDead) return;

    // 2. 피격 사운드 랜덤 재생 (Sound Pool)
    if (HitSoundPool.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, HitSoundPool.Num() - 1);
        if (HitSoundPool[RandomIndex])
        {
            UGameplayStatics::PlaySoundAtLocation(this, HitSoundPool[RandomIndex], GetActorLocation());
        }
    }

    // 3. 무기 UI 위에 덮어씌운 피격 이펙트 위젯 재생!
    if (HitFlipbook && WeaponWidgetInstance)
    {
        WeaponWidgetInstance->PlayHitEffect(HitFlipbook);
    }
}

void ADelveCharacter::HandleDeath(ACharacter* DeadCharacter)
{
    UE_LOG(LogTemp, Error, TEXT("Player Died!"));

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        // 1. 캐릭터 조작 완전 차단
        DisableInput(PC);

        // 2. 걷던 관성 정지
        if (UCharacterMovementComponent* MovementComp = FindComponentByClass<UCharacterMovementComponent>())
        {
            MovementComp->Velocity = FVector::ZeroVector;
        }

        // 3. 맵에 있는 모든 적(나를 제외한 캐릭터) 즉사시키기!
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundActors);

        for (AActor* Actor : FoundActors)
        {
            ACharacter* OtherChar = Cast<ACharacter>(Actor);
            
            // 찾은 캐릭터가 유효하고, '나 자신(플레이어)'이 아니라면
            if (OtherChar && OtherChar != this && OtherChar->IsA(ADelveBoss::StaticClass()))
            {
                // 99999의 절명 데미지를 가하여 적의 HealthHandler가 사망 처리를 하도록 유도합니다.
                UGameplayStatics::ApplyDamage(OtherChar, 99999.0f, PC, this, nullptr);
            }
        }

        // 4. 기존에 재생 중이던 모든 배경음/효과음 강제 종료!
        for (TObjectIterator<UAudioComponent> It; It; ++It)
        {
            if (It->GetWorld() == GetWorld() && It->IsPlaying())
            {
                It->Stop();
            }
        }

        if (this->CurrentBGMComponent)
        {
            this->CurrentBGMComponent->Stop();
        }

        // 5. 사망 사운드 및 전용 BGM 재생
        if (PlayerDeadSound)
        {
            UGameplayStatics::PlaySound2D(this, PlayerDeadSound);
        }
        if (PlayerDeadBGMSound)
        {
            UGameplayStatics::PlaySound2D(this, PlayerDeadBGMSound);
        }

        // 6. 게임 오버 UI 띄우기 (스페이스바 포커스 포함)
        if (GameOverWidgetClass)
        {
            UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
            if (GameOverWidget)
            {
                GameOverWidget->AddToViewport();
                GameOverWidget->SetKeyboardFocus(); // 스페이스바 인식용 포커스

                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
        }
    }
}

void ADelveCharacter::Input_InteractPressed()
{
    if (!FirstPersonCamera) return;

    FVector StartLoc = FirstPersonCamera->GetComponentLocation();
    FVector EndLoc = StartLoc + (FirstPersonCamera->GetForwardVector() * InteractRange);

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility, CollisionParams);

    //
    //DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, 2.0f, 0, 1.0f);

    if (bHit && HitResult.GetActor())
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor->Implements<UInteractable>())
        {
            IInteractable::Execute_Interact(HitActor, this);
            UE_LOG(LogTemp, Display, TEXT("%s와 상호작용 성공!"), *HitActor->GetName());
        }
    }
}

void ADelveCharacter::Input_PotionPressed()
{
    if (CombatHandler) 
    {
        // EWeaponSkillSlot::Potion 슬롯에 할당된 스킬을 실행!
        CombatHandler->HandleInput(EWeaponSkillSlot::One, true); 
    }
}

void ADelveCharacter::Input_MapPressed()
{
    if (!FullMapWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("전체 맵 위젯 클래스가 블루프린트에 설정되지 않았습니다!"));
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    // 위젯이 아직 생성되지 않았다면 최초 1회 생성합니다.
    if (!FullMapWidgetInstance)
    {
        FullMapWidgetInstance = CreateWidget<UDungeonFullMapWidget>(GetWorld(), FullMapWidgetClass);
    }

    if (FullMapWidgetInstance)
    {
        // 이미 화면에 띄워져 있다면 닫기
        if (FullMapWidgetInstance->IsInViewport())
        {
            FullMapWidgetInstance->RemoveFromParent();
            
            // 게임 조작 모드로 복귀 및 마우스 숨김
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
        }
        // 화면에 없다면 열기
        else
        {
            FullMapWidgetInstance->AddToViewport(10); // 미니맵보다 높게 설정
            
            // UI 조작 모드로 변경 (필요하다면 GameAndUI 사용) 및 마우스 표시
            PC->SetInputMode(FInputModeGameAndUI());
            PC->bShowMouseCursor = true;
        }
    }
}