#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "WeaponDisplayWidget.h" // 위젯 헤더 추가
#include "DelveCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class AWeaponBase;

UCLASS()
class DELVEINTO_API ADelveCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADelveCharacter();

protected:
    virtual void BeginPlay() override;

public:	
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // --- 컴포넌트 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    UCameraComponent* FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
    USceneComponent* WeaponAttachPoint;

    // --- UI (추가됨) ---
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UWeaponDisplayWidget> WeaponWidgetClass; // 에디터에서 WBP_WeaponHUD 할당

    UPROPERTY()
    UWeaponDisplayWidget* WeaponWidgetInstance;

    // --- 입력 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* AttackAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* ChargeAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* SkillQAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* SkillEAction;

    // --- 무기 ---
    UPROPERTY(EditAnywhere, Category = Combat)
    TSubclassOf<AWeaponBase> StartingWeaponClass;

    UPROPERTY(VisibleInstanceOnly, Category = Combat)
    AWeaponBase* CurrentWeapon;

    // --- 무기 UI 업데이트 함수 ---
    void UpdateWeaponUI(UPaperFlipbook* NewFlipbook, bool bLoop);

    // --- 차징 관련 변수 (추가) ---
    UPROPERTY(EditAnywhere, Category = "Combat | Charge")
    TSubclassOf<class ADelveProjectile> SwordWaveClass; // 에디터에서 생성한 BP_SwordWave 할당

    UPROPERTY(EditAnywhere, Category = "Combat | Charge")
    float MaxChargeTime = 1.5f; // 1.5초면 풀차징

private:
    float ChargeStartTime = 0.0f;
    bool bIsCharging = false;
    
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void OnPrimaryAttack();
    void OnSecondaryAttack(const FInputActionValue& Value);
    void FinalizeSecondaryAttack(); // 차징 완료 후 실제 발사 로직
    void OnSkillQ();
    void OnSkillE();
};