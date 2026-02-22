#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DelveCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UCombatHandler;
class UHealthHandler;
class UInventoryHandler; // [신규] 인벤토리 전방 선언
class UHandDisplayWidget;

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

    // --- 데미지 처리 ---
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // --- 컴포넌트 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FirstPersonCamera;

    // 전투 총괄 핸들러
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCombatHandler* CombatHandler;

    // 체력 총괄 핸들러
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthHandler* HealthHandler;

    // [신규] 인벤토리 총괄 핸들러
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInventoryHandler* InventoryHandler;

    // --- UI ---
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UHandDisplayWidget> WeaponWidgetClass; 

    UPROPERTY()
    UHandDisplayWidget* WeaponWidgetInstance;

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

private:
    // --- 입력 콜백 함수들 ---
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

    // --- HealthHandler 이벤트 수신용 함수들 ---
    UFUNCTION()
    void HandleDamaged(float InMaxHealth, float InCurrentHealth);

    UFUNCTION()
    void HandleDeath(ACharacter* DeadCharacter);

    // [추가] 상호작용 입력 콜백 함수
    void Input_InteractPressed();
    
    // [추가] 상호작용 최대 거리
    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
    float InteractRange = 300.0f;
};