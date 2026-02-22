#include "DelveCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// [신규 시스템 헤더들]
#include "Components/CapsuleComponent.h"
#include "NewSystem/Widgets/HandDisplayWidget.h"
#include "NewSystem/Widgets/HealthBarWidget.h"
#include "Handlers/CombatHandler.h"
#include "Handlers/HealthHandler.h"
#include "Handlers/InventoryHandler.h" 
// [신규] 퍽 관련 헤더 추가 (경로는 실제 프로젝트 구조에 맞춰주세요)
#include "NewSystem/Entities/Characters/Handlers/PerkHandler.h"
#include "NewSystem/Interfaces/Interactable.h"
#include "NewSystem/Widgets/Perks/PerkSelectionWidget.h"

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
    
    // [신규] 퍽 핸들러 생성
    PerkHandler = CreateDefaultSubobject<UPerkHandler>(TEXT("PerkHandler"));
}

void ADelveCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    if (HealthHandler)
    {
        HealthHandler->OnDamaged.AddDynamic(this, &ADelveCharacter::HandleDamaged);
        HealthHandler->OnDeath.AddDynamic(this, &ADelveCharacter::HandleDeath);
    }

    // UI 생성 (기존 로직 유지)
    if (WeaponWidgetClass)
    {
        WeaponWidgetInstance = CreateWidget<UHandDisplayWidget>(GetWorld(), WeaponWidgetClass);
        if (WeaponWidgetInstance)
        {
            WeaponWidgetInstance->AddToViewport();
            
            if (HealthHandler && WeaponWidgetInstance->HealthBar)
            {
                WeaponWidgetInstance->HealthBar->UpdateHealthRatio(HealthHandler->CurrentHealth / HealthHandler->MaxHealth);
            }
            
            CombatHandler->Initialize(WeaponWidgetInstance);
        }
    }
}

// -------------------------------------------------------------
// [신규] 퍽 시스템: 레벨업 시 UI 띄우기
// -------------------------------------------------------------
void ADelveCharacter::TriggerLevelUp()
{
    if (!PerkSelectionWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PerkSelectionWidgetClass가 설정되지 않았습니다! (캐릭터 블루프린트 확인)"));
        return;
    }

    UPerkSelectionWidget* SelectionUI = CreateWidget<UPerkSelectionWidget>(GetWorld(), PerkSelectionWidgetClass);
    if (SelectionUI)
    {
        SelectionUI->AddToViewport();
        // 3개의 선택지를 띄우고, PerkHandler를 넘겨줍니다.
        SelectionUI->ShowChoices(PerkHandler, 3);
    }
}

// 콘솔창에서 바로 호출 가능한 테스트용 함수
void ADelveCharacter::DebugLevelUp()
{
    TriggerLevelUp();
}

// -------------------------------------------------------------
// 이하 기존 입력 바인딩 로직들은 동일하게 유지
// -------------------------------------------------------------
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
    if (WeaponWidgetInstance && WeaponWidgetInstance->HealthBar)
    {
        WeaponWidgetInstance->HealthBar->UpdateHealthRatio(InCurrentHealth / InMaxHealth);
    }
}

void ADelveCharacter::HandleDeath(ACharacter* DeadCharacter)
{
    UE_LOG(LogTemp, Error, TEXT("Player Died!"));

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
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

    DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, 2.0f, 0, 1.0f);

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