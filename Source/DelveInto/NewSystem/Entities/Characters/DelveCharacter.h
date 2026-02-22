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
class UInventoryHandler;
class UHandDisplayWidget;
// [신규] 퍽 시스템 관련 전방 선언
class UPerkHandler;
class UPerkSelectionWidget;

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCombatHandler* CombatHandler;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthHandler* HealthHandler;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInventoryHandler* InventoryHandler;

    // [신규] 퍽 총괄 핸들러 추가
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPerkHandler* PerkHandler;

    // --- UI ---
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UHandDisplayWidget> WeaponWidgetClass; 

    UPROPERTY()
    UHandDisplayWidget* WeaponWidgetInstance;

    // [신규] 퍽 선택 창 UI 클래스
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UPerkSelectionWidget> PerkSelectionWidgetClass;

    // --- 진행(Progression) 로직 ---
    // [신규] 레벨업 시 UI를 띄우는 함수
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void TriggerLevelUp();

    // [신규] 콘솔 창에서 "DebugLevelUp"을 입력하여 즉시 테스트할 수 있는 디버그 함수
    UFUNCTION(Exec)
    void DebugLevelUp();

    // --- 입력 ---
    // (기존 입력 변수들은 그대로 유지...)
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

    // --- 상호작용 ---
    void Input_InteractPressed();
    
    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
    float InteractRange = 300.0f;
};