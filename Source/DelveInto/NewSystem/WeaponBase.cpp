#include "WeaponBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DelveCharacter.h" // 캐릭터 함수 호출용
#include "Camera/CameraComponent.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // 컴포넌트는 만들지만 실제 화면에는 그리지 않음 (UI가 대신 그림)
    WeaponFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("WeaponFlipbook"));
    WeaponFlipbook->SetupAttachment(RootComponent);
    WeaponFlipbook->SetHiddenInGame(true); // 게임에선 숨김
}

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();
    // 시작 시 조금 늦게 Idle 호출 (주인이 UI 생성할 시간 확보)
    FTimerHandle InitHandle;
    GetWorld()->GetTimerManager().SetTimer(InitHandle, this, &AWeaponBase::ReturnToIdle, 0.1f, false);
}

// --- UI 애니메이션 요청 로직 ---

float AWeaponBase::PlayAnimationBySlot(EWeaponSkillSlot Slot)
{
    if (TObjectPtr<UPaperFlipbook>* FoundAnim = SkillAnimations.Find(Slot))
    {
        return PlayAnimationDirectly(*FoundAnim);
    }
    return 0.0f;
}

float AWeaponBase::PlayAnimationDirectly(UPaperFlipbook* InFlipbook)
{
    if (InFlipbook)
    {
        // 주인(Character)에게 UI 업데이트 요청
        if (ADelveCharacter* MyOwner = Cast<ADelveCharacter>(GetOwner()))
        {
            // 공격 모션은 Loop 끔
            MyOwner->UpdateWeaponUI(InFlipbook, false);
        }
        
        bIsAttacking = true;
        return InFlipbook->GetTotalDuration();
    }
    return 0.0f;
}

void AWeaponBase::ReturnToIdle()
{
    if (IdleFlipbook)
    {
        if (ADelveCharacter* MyOwner = Cast<ADelveCharacter>(GetOwner()))
        {
            // Idle 모션은 Loop 켬
            MyOwner->UpdateWeaponUI(IdleFlipbook, true);
        }
    }
    bIsAttacking = false;
}

// --- 쿨타임 및 전투 로직 (기존 동일) ---

bool AWeaponBase::IsOnCooldown(EWeaponSkillSlot Slot)
{
    if (CooldownMap.Contains(Slot))
    {
        return GetWorld()->GetTimeSeconds() < CooldownMap[Slot];
    }
    return false;
}

void AWeaponBase::SetCooldown(EWeaponSkillSlot Slot, float Duration)
{
    CooldownMap.Add(Slot, GetWorld()->GetTimeSeconds() + Duration);
}

void AWeaponBase::ApplyDamageSphere(float Damage, float Radius, FVector OriginOffset)
{
    FVector Start = GetActorLocation() + (GetActorForwardVector() * OriginOffset.X) + 
                    (GetActorRightVector() * OriginOffset.Y) + 
                    (GetActorUpVector() * OriginOffset.Z);
    
    DrawDebugSphere(GetWorld(), Start, Radius, 12, FColor::Red, false, 1.0f);

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(
        this, Start, Radius, ObjectTypes, nullptr, TArray<AActor*>(), OverlappedActors
    );

    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor && HitActor != GetOwner())
        {
            FDamageEvent DamageEvent;
            HitActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
            UE_LOG(LogTemp, Log, TEXT("Hit: %s, Damage: %f"), *HitActor->GetName(), Damage);
        }
    }
}

void AWeaponBase::ApplyDamageFanShape(float Damage, float Radius, float HalfAngle, float HalfHeight)
{
    UWorld* World = GetWorld();
    if (!World) return;

    FVector Origin = GetActorLocation();
    FVector Forward = GetActorForwardVector();

    // --- 1. 디버그 그리기 (시각화) ---
    // A. 전체 원기둥 그리기 (투명한 회색)
    DrawDebugCylinder(World, Origin - FVector(0,0,HalfHeight), Origin + FVector(0,0,HalfHeight), Radius, 12, FColor::Silver, false, 1.0f);

    // B. 부채꼴 각도 표시 (왼쪽/오른쪽 제한선)
    // Z축 회전만 적용하여 좌우 각도 벡터 계산
    FVector LeftDir = Forward.RotateAngleAxis(-HalfAngle, FVector::UpVector);
    FVector RightDir = Forward.RotateAngleAxis(HalfAngle, FVector::UpVector);

    // 선 그리기 (빨간색 - 부채꼴 범위)
    DrawDebugLine(World, Origin, Origin + (LeftDir * Radius), FColor::Red, false, 1.0f);
    DrawDebugLine(World, Origin, Origin + (RightDir * Radius), FColor::Red, false, 1.0f);
    // -----------------------------

    // --- 2. 캡슐 오버랩으로 범위 내 적 찾기 (Broad Phase) ---
    // Sphere 대신 Capsule을 써서 "기둥" 모양으로 찾습니다.
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::CapsuleOverlapActors(
        this,
        Origin,
        Radius,
        HalfHeight, // 높이 반영
        ObjectTypes,
        nullptr,
        TArray<AActor*>(),
        OverlappedActors
    );

    // --- 3. 각도 판정 (Narrow Phase) ---
    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor && HitActor != GetOwner())
        {
            // A. 방향 벡터 (높이 무시하고 2D 평면으로 투영)
            FVector ToTarget = HitActor->GetActorLocation() - Origin;
            ToTarget.Z = 0.0f; // 높이 차이 제거
            
            FVector FlatForward = Forward;
            FlatForward.Z = 0.0f;

            // 정규화
            ToTarget.Normalize();
            FlatForward.Normalize();

            // B. 각도 계산 (Dot Product)
            float DotValue = FVector::DotProduct(FlatForward, ToTarget);
            float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(DotValue));

            // C. 각도 체크
            if (AngleDegree <= HalfAngle)
            {
                // 적중!
                FDamageEvent DamageEvent;
                HitActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
                UE_LOG(LogTemp, Log, TEXT("Fan Hit: %s (Angle: %f)"), *HitActor->GetName(), AngleDegree);
            }
        }
    }
}

void AWeaponBase::ApplyDamageSphericalCone(float Damage, float Radius, float HalfAngle)
{
    UWorld* World = GetWorld();
    if (!World) return;

    // 1. 판정 기준점 잡기 (중요: 카메라 위치!)
    FVector Origin;
    FVector Forward;

    // 주인이 캐릭터라면 카메라 정보를 가져옵니다.
    if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
    {
        // 1인칭 카메라는 보통 Mesh가 아니라 CameraComponent 위치를 씁니다.
        // ADelveCharacter 헤더를 include 하거나, FindComponentByClass를 씁니다.
        if (UCameraComponent* Cam = OwnerChar->FindComponentByClass<UCameraComponent>())
        {
            Origin = Cam->GetComponentLocation();
            Forward = Cam->GetForwardVector();
        }
        else
        {
            // 카메라 없으면 눈높이로 대체
            Origin = OwnerChar->GetPawnViewLocation();
            Forward = OwnerChar->GetActorForwardVector();
        }
    }
    else
    {
        Origin = GetActorLocation();
        Forward = GetActorForwardVector();
    }

    // 2. 디버그 그리기 (손전등 모양 Cone)
    // DrawDebugCone은 원뿔을 그립니다. 끝부분이 평평하지만, 실제 판정은 구형으로 할 겁니다.
    DrawDebugCone(
        World,
        Origin,
        Forward,
        Radius,
        FMath::DegreesToRadians(HalfAngle), // 각도
        FMath::DegreesToRadians(HalfAngle),
        12,
        FColor::Orange, // 주황색으로 표시
        false,
        1.0f
    );
    // 구형 끝부분을 표현하기 위해 원도 하나 그립니다 (옵션)
    // DrawDebugSphere(World, Origin, Radius, 12, FColor::Orange, false, 1.0f);


    // 3. Broad Phase: 일단 사거리(구) 안에 있는 애들을 다 찾습니다.
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    UKismetSystemLibrary::SphereOverlapActors(
        this,
        Origin,
        Radius,
        ObjectTypes,
        nullptr,
        TArray<AActor*>(),
        OverlappedActors
    );

    // 4. Narrow Phase: 각도 계산 (3D)
    for (AActor* HitActor : OverlappedActors)
    {
        if (HitActor && HitActor != GetOwner())
        {
            // A. 방향 벡터 (3D 그대로 사용! Z축 무시 안 함)
            FVector ToTarget = HitActor->GetActorLocation() - Origin;
            
            // B. 거리 재확인 (SphereOverlap은 정확하지만 한 번 더 확실하게)
            // (이미 SphereOverlap을 했으므로 생략 가능하지만, 정밀함을 위해 남겨둠)
            if (ToTarget.SizeSquared() > Radius * Radius) continue;

            // 정규화 (길이 1로)
            ToTarget.Normalize();

            // C. 3D 각도 계산 (Dot Product)
            // 내적값 1.0 = 정면, 0.0 = 90도, -1.0 = 뒤
            float DotValue = FVector::DotProduct(Forward, ToTarget);
            
            // 각도로 변환
            float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(DotValue));

            // D. 각도 체크
            if (AngleDegree <= HalfAngle)
            {
                // 적중!
                FDamageEvent DamageEvent;
                HitActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
                UE_LOG(LogTemp, Log, TEXT("3D Cone Hit: %s (Angle: %f)"), *HitActor->GetName(), AngleDegree);
            }
        }
    }
}