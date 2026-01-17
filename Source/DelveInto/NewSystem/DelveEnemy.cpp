#include "DelveEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ADelveEnemy::ADelveEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 스프라이트 컴포넌트 설정
    EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
    EnemyFlipbook->SetupAttachment(RootComponent);
    // 스프라이트 위치 조정 (캡슐 중앙에 오도록)
    EnemyFlipbook->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); 
    EnemyFlipbook->SetRelativeRotation(FRotator(0.f, -90.f, 0.f)); // 기본 정면

    // 2. AI 이동 설정
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 캡슐 회전

    // 3. 충돌 설정 (Pawn)
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
}

void ADelveEnemy::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void ADelveEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead) return;

    UpdateBillboardRotation();
    DetermineSpriteDirection(); // 여기서 정면/측면/후면 그림 교체
}

float ADelveEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.0f;

    // 1. 기존 데미지 로직 실행
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);
    
    // 2. [추가] 피격 시각 효과 (빨간색)
    if (EnemyFlipbook)
    {
        // 색상을 빨간색으로 변경 (RGB: 1, 0, 0 / A: 1)
        EnemyFlipbook->SetSpriteColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));

        // 기존 타이머가 돌고 있다면 초기화 (연타 맞았을 때 빨간색 유지)
        GetWorld()->GetTimerManager().ClearTimer(HitFlashTimer);

        // 0.5초 뒤에 원래 색으로 돌아오게 타이머 설정
        GetWorld()->GetTimerManager().SetTimer(HitFlashTimer, this, &ADelveEnemy::ResetSpriteColor, 0.5f, false);
    }

    // 3. 사망 처리
    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (GetController()) GetController()->StopMovement();

        // (선택 사항) 사망 시에는 즉시 원래 색으로 돌리거나, 빨간 상태로 죽게 둘 수 있습니다.
        // 여기서는 깔끔하게 원래 색으로 돌리고 죽는 모션 재생
        ResetSpriteColor(); 

        UE_LOG(LogTemp, Warning, TEXT("Enemy Died!"));
        
        // 사망 애니메이션 재생 로직 (PaperZD 등)
    }

    return ActualDamage;
}

void ADelveEnemy::ResetSpriteColor()
{
    if (EnemyFlipbook)
    {
        // 원래 색(흰색 = 텍스처 원본 색)으로 복귀
        EnemyFlipbook->SetSpriteColor(FLinearColor::White);
    }
}

void ADelveEnemy::UpdateBillboardRotation()
{
    // 카메라 위치 가져오기
    if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
    {
        FVector CameraLoc = CameraManager->GetCameraLocation();
        FVector EnemyLoc = GetActorLocation();

        // 몬스터가 카메라를 바라보는 회전값(Yaw) 계산
        FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(EnemyLoc, CameraLoc);
        
        // 캡슐은 이동방향을 보지만, 그림(Flipbook)은 항상 카메라를 봐야 함
        // 단, Pitch/Roll은 고정하고 Yaw(좌우)만 회전
        EnemyFlipbook->SetWorldRotation(FRotator(0.0f, LookAtRot.Yaw - 90.0f, 0.0f)); 
        // Note: -90.0f는 스프라이트 원본이 어느 방향을 보고 있느냐에 따라 보정 필요 (보통 Paper2D는 Y축 기준이라)
    }
}

void ADelveEnemy::DetermineSpriteDirection()
{
    // 여기에 기획하신 "정면, 45도, 90도, 후면" 계산 로직이 들어갑니다.
    // 몬스터의 진행 방향(GetActorForwardVector)과 카메라 방향 사이의 각도를 계산합니다.
    
    // 지금은 복잡함을 줄이기 위해 비워두거나, 
    // 추후 PaperZD의 "Directional Sprite" 기능을 쓰면 코딩 없이 자동화 가능합니다.
}