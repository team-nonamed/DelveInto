#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NewSystem/Items/ItemData.h"
#include "Widgets/Maps/DungeonFullMapWidget.h"
#include "DelveCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UCombatHandler;
class UHealthHandler;
class UInventoryHandler;
class UHandDisplayWidget;
class UPerkHandler;
class UPerkSelectionWidget;
class UPaperFlipbook;

enum class EStatCategory : uint8;

UCLASS(Abstract, Blueprintable)
class DELVEINTO_API ADelveCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADelveCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // --- 컴포넌트 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCombatHandler* CombatHandler;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthHandler* HealthHandler;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInventoryHandler* InventoryHandler;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPerkHandler* PerkHandler;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UItemData* PotionData;

    // --- UI ---
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UHandDisplayWidget> WeaponWidgetClass; 

    UPROPERTY()
    UHandDisplayWidget* WeaponWidgetInstance;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UPerkSelectionWidget> PerkSelectionWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> GameOverWidgetClass;
    
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UDungeonFullMapWidget> FullMapWidgetClass;

    UPROPERTY()
    UDungeonFullMapWidget* FullMapWidgetInstance;

    // --- 오디오 및 애니메이션 ---
    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* PlayerDeadSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* PlayerDeadBGMSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TArray<USoundBase*> HitSoundPool; 

    UPROPERTY(EditAnywhere, Category = "Animation")
    UPaperFlipbook* HitFlipbook; 

    // --- 진행(Progression) 로직 ---
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void TriggerLevelUp();

    UFUNCTION(Exec)
    void DebugLevelUp();

    // --- 입력 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* PrimaryAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* SecondaryAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* SkillQAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* SkillEAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* DashAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* InteractAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* OneAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MapAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAudioComponent* CurrentBGMComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool BossTriggered = false;

protected:
    UFUNCTION()
    void HandleStatChanged(EStatCategory StatType, float DeltaValue);

    UPROPERTY()
    float BaseWalkSpeed;

    UPROPERTY()
    float CurrentMoveSpeedMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
    float InteractRange = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
    USoundBase* PerkSound;

private:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    
    void Input_PrimaryPressed();
    void Input_PrimaryReleased();
    void Input_SecondaryPressed();
    void Input_SecondaryReleased();
    void Input_SkillQPressed();
    void Input_SkillQReleased();
    void Input_SkillEPressed();
    void Input_SkillEReleased();
    void Input_JumpPressed();
    void Input_DashPressed();
    void Input_DashReleased();
    void Input_InteractPressed();
    void Input_PotionPressed();
    void Input_MapPressed();
    
    UFUNCTION()
    void HandleDamaged(float InMaxHealth, float InCurrentHealth);

    UFUNCTION()
    void HandleDeath(ACharacter* DeadCharacter);
};