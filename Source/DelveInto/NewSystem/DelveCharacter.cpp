#include "DelveCharacter.h"

#include "DelveProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WeaponBase.h"

ADelveCharacter::ADelveCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    WeaponAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponAttachPoint"));
    WeaponAttachPoint->SetupAttachment(FirstPersonCamera);
    // UI 방식이므로 AttachPoint 위치는 실제로는 중요하지 않지만 로직상 남겨둡니다.
    WeaponAttachPoint->SetRelativeLocation(FVector(50.f, 20.f, -20.f)); 
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

    // [UI 생성]
    if (WeaponWidgetClass)
    {
        WeaponWidgetInstance = CreateWidget<UWeaponDisplayWidget>(GetWorld(), WeaponWidgetClass);
        if (WeaponWidgetInstance)
        {
            WeaponWidgetInstance->AddToViewport();
        }
    }

    // 무기 스폰
    if (StartingWeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = this;
        CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartingWeaponClass, WeaponAttachPoint->GetComponentTransform(), SpawnParams);
        CurrentWeapon->MyOwnerCharacter = this;
        if (CurrentWeapon)
        {
            CurrentWeapon->AttachToComponent(WeaponAttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        }
    }
}

// [무기가 호출하는 함수]
void ADelveCharacter::UpdateWeaponUI(UPaperFlipbook* NewFlipbook, bool bLoop)
{
    if (WeaponWidgetInstance)
    {
        WeaponWidgetInstance->PlayFlipbook(NewFlipbook, bLoop);
    }
}

// --- 입력 바인딩 ---
void ADelveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADelveCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADelveCharacter::Look);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADelveCharacter::OnPrimaryAttack);
        EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Started, this, &ADelveCharacter::OnSecondaryAttack);
        EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Completed, this, &ADelveCharacter::OnSecondaryAttack);
        EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Started, this, &ADelveCharacter::OnSkillQ);
        EnhancedInputComponent->BindAction(SkillEAction, ETriggerEvent::Started, this, &ADelveCharacter::OnSkillE);
    }
}

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

void ADelveCharacter::OnPrimaryAttack() { if (CurrentWeapon) CurrentWeapon->TryPrimaryAttack(); }
void ADelveCharacter::OnSecondaryAttack(const FInputActionValue& Value)
{
    // 버튼을 막 눌렀을 때 (Started 대용)
    if (!bIsCharging)
    {
        bIsCharging = true;
        ChargeStartTime = GetWorld()->GetTimeSeconds();
        
        // 무기에게 차징 시작 알림 (애니메이션 재생용)
        if (CurrentWeapon) CurrentWeapon->TrySecondaryAttack(true);
    }
    // 버튼을 뗐을 때 (Completed 시점)
    else
    {
        FinalizeSecondaryAttack();
    }
}

void ADelveCharacter::FinalizeSecondaryAttack()
{
    if (!bIsCharging) return;

    bIsCharging = false;
    float HeldTime = GetWorld()->GetTimeSeconds() - ChargeStartTime;
    
    // 차징 비율 계산 (0.0 ~ 1.0)
    float ChargeRatio = FMath::Clamp(HeldTime / MaxChargeTime, 0.0f, 1.0f);

    // 무기에게 차징 종료 알림 (발사 애니메이션 등)
    if (CurrentWeapon) CurrentWeapon->TrySecondaryAttack(false);

    // 투사체(검기) 생성 로직
    if (SwordWaveClass)
    {
        // 카메라 앞 방향으로 발사 위치 설정
        FVector SpawnLocation = FirstPersonCamera->GetComponentLocation() + (FirstPersonCamera->GetForwardVector() * 100.0f);
        FRotator SpawnRotation = FirstPersonCamera->GetComponentRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = this;

        ADelveProjectile* Projectile = GetWorld()->SpawnActor<ADelveProjectile>(SwordWaveClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (Projectile)
        {
            // [핵심] 차징된 비율을 투사체에 전달
            Projectile->InitializeChargeStats(ChargeRatio);
        }
    }
}
void ADelveCharacter::OnSkillQ() { if (CurrentWeapon) CurrentWeapon->TrySkillQ(); }
void ADelveCharacter::OnSkillE() { if (CurrentWeapon) CurrentWeapon->TrySkillE(); }