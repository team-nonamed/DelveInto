#include "DelveCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// [신규 시스템 헤더들]
#include "Components/CapsuleComponent.h"
#include "NewSystem/Widgets/HandDisplayWidget.h"
#include "NewSystem/Widgets/HealthBarWidget.h" // HealthBar 업데이트용
#include "Handlers/CombatHandler.h"
#include "Handlers/HealthHandler.h"

ADelveCharacter::ADelveCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // 캡슐 초기화
    GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);

    // 카메라 생성 및 부착
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    // [핵심] 핵심 핸들러 컴포넌트 생성
    CombatHandler = CreateDefaultSubobject<UCombatHandler>(TEXT("CombatHandler"));
    HealthHandler = CreateDefaultSubobject<UHealthHandler>(TEXT("HealthHandler"));
}

void ADelveCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 1. 입력 매핑 컨텍스트 추가
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // 2. 체력 핸들러 이벤트 바인딩
    if (HealthHandler)
    {
        HealthHandler->OnDamaged.AddDynamic(this, &ADelveCharacter::HandleDamaged);
        HealthHandler->OnDeath.AddDynamic(this, &ADelveCharacter::HandleDeath);
    }

    // 3. UI 생성 및 CombatHandler 초기화
    if (WeaponWidgetClass)
    {
        WeaponWidgetInstance = CreateWidget<UHandDisplayWidget>(GetWorld(), WeaponWidgetClass);
        if (WeaponWidgetInstance)
        {
            WeaponWidgetInstance->AddToViewport();
            
            // UI 생성 직후 현재 체력비율을 한 번 갱신해줌
            if (HealthHandler && WeaponWidgetInstance->HealthBar)
            {
                WeaponWidgetInstance->HealthBar->UpdateHealthRatio(HealthHandler->CurrentHealth / HealthHandler->MaxHealth);
            }
            
            // 컴뱃 핸들러에 위젯 제어권 넘김 (이 안에서 기본 무기 세팅도 이루어짐)
            CombatHandler->Initialize(WeaponWidgetInstance);
        }
    }
}

// --- 입력 바인딩 ---
void ADelveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // 이동 / 시점
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADelveCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADelveCharacter::Look);
        
        // 스킬 액션 바인딩 (Started = Pressed, Completed = Released)
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
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADelveCharacter::Input_DashReleased);
    }
}

// --- 입력 라우팅 로직 ---
// 입력을 받아 즉시 CombatHandler로 전달합니다.
void ADelveCharacter::Input_PrimaryPressed()   {
    UE_LOG(LogTemp, Warning, TEXT("좌클릭 입력 들어옴!"));
    if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Primary, true);
}
void ADelveCharacter::Input_PrimaryReleased()  { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Primary, false); }

void ADelveCharacter::Input_SecondaryPressed()
{
    UE_LOG(LogTemp, Display, TEXT("우클릭 입력 들어옴!"))
    if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Secondary, true);
}
void ADelveCharacter::Input_SecondaryReleased(){ if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::Secondary, false); }

void ADelveCharacter::Input_SkillQPressed()    { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillQ, true); }
void ADelveCharacter::Input_SkillQReleased()   { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillQ, false); }

void ADelveCharacter::Input_SkillEPressed()    { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillE, true); }
void ADelveCharacter::Input_SkillEReleased()   { if (CombatHandler) CombatHandler->HandleInput(EWeaponSkillSlot::SkillE, false); }

void ADelveCharacter::Input_JumpPressed()
{
    if (CombatHandler)
    {
        CombatHandler->HandleInput(EWeaponSkillSlot::Jump, true);
    }
}

void ADelveCharacter::Input_DashPressed()
{
    UE_LOG(LogTemp, Warning, TEXT("좌클릭 입력 들어옴!"));
    
    if (CombatHandler)
    {
        CombatHandler->HandleInput(EWeaponSkillSlot::Dash, true);
    }
}

void ADelveCharacter::Input_DashReleased()
{
    if (CombatHandler)
    {
        CombatHandler->HandleInput(EWeaponSkillSlot::Dash, false);
    }
}




// --- 이동 및 시점 ---
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

    // [디버그] 매 프레임 체력 상태 표시 (HealthHandler를 참조)
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

// --- 데미지 및 체력 이벤트 처리 ---
float ADelveCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 체력 감소 처리를 HealthHandler에게 위임
    if (HealthHandler)
    {
        ActualDamage = HealthHandler->ApplyDamage(ActualDamage);
    }

    return ActualDamage;
}

// HealthHandler에서 체력이 깎였다고 방송(Broadcast)하면 실행됨
void ADelveCharacter::HandleDamaged(float InMaxHealth, float InCurrentHealth)
{
    // UI 업데이트
    if (WeaponWidgetInstance && WeaponWidgetInstance->HealthBar)
    {
        WeaponWidgetInstance->HealthBar->UpdateHealthRatio(InCurrentHealth / InMaxHealth);
    }
}

// HealthHandler에서 체력이 0이 되어 사망했다고 방송(Broadcast)하면 실행됨
void ADelveCharacter::HandleDeath(ACharacter* DeadCharacter)
{
    UE_LOG(LogTemp, Error, TEXT("Player Died!"));

    // 1. 조작 차단
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    // 2. 추가적인 사망 연출 (애니메이션, 래그돌 등)을 여기에 구현
}