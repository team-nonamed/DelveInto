#include "DashSlash.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h" // 디버그 선을 그리기 위해 추가 (출시 전 삭제)

USkill_DashSlash::USkill_DashSlash()
{
    // 기획하신 스펙 기본값 세팅
    SkillName = FText::FromString(TEXT("돌진 베기 (Q)"));
    BaseDamage = 30.0f;
    MaxCooldownSeconds = 15.0f;
    
    // 컴포넌트 세팅 보장
    bHasCooldown = true;
    bHasCombo = false;
    bIsChargingSkill = false;
}

bool USkill_DashSlash::ExecuteSkillLogic_Implementation()
{
    // 1. 시전자(캐릭터) 정보 가져오기
    // (USkillBase는 ActorComponent이므로 GetOwner()가 시전자입니다)
    ACharacter* Caster = Cast<ACharacter>(GetOwner());
    if (!Caster) 
    {
        UE_LOG(LogSkill, Error, TEXT("USkill_DashSlash의 Owner가 Character가 아닙니다!"));
        return false;
    }

    UWorld* World = GetWorld();
    if (!World) return false;

    FVector StartLoc = Caster->GetActorLocation();
    FVector ForwardDir = Caster->GetActorForwardVector();

    // =========================================================
    // 1. 돌진 (Dash) 액션
    // =========================================================
    // XY축의 기존 속도를 덮어쓰고 강하게 발사! (Z축은 덮어쓰지 않아 자연스러운 낙하 유지)
    Caster->LaunchCharacter(ForwardDir * DashForce, true, false);

    // =========================================================
    // 2. 타격 판정 (Sweep)
    // =========================================================
    FVector EndLoc = StartLoc + (ForwardDir * SlashRange);
    FCollisionShape SlashShape = FCollisionShape::MakeSphere(SlashRadius);
    
    TArray<FHitResult> HitResults;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Caster); // 자기 자신 타격 무시

    // 전방으로 두꺼운 구체를 훑어서 충돌체들을 찾음
    bool bHit = World->SweepMultiByChannel(
        HitResults, 
        StartLoc, 
        EndLoc, 
        FQuat::Identity, 
        ECC_Pawn, // 적 몬스터의 콜리전 채널 (필요에 따라 몬스터 전용 채널로 변경하세요)
        SlashShape, 
        Params
    );

    // (디버그용) 실제 베기 판정이 어떻게 나가는지 빨간색 캡슐로 그려줍니다. 나중에 주석 처리하세요.
    DrawDebugCapsule(World, StartLoc + (EndLoc - StartLoc)*0.5f, SlashRange*0.5f, SlashRadius, FRotationMatrix::MakeFromZ(ForwardDir).ToQuat(), FColor::Red, false, 1.0f);

    // =========================================================
    // 3. 적중 처리 및 방송 (SkillEffect 연동부)
    // =========================================================
    if (bHit)
    {
        // 내장된 CalculateDamage()를 통해 데미지 산출 (난수 등 모두 자동 반영됨)
        float FinalDamage = CalculateDamage();

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor && HitActor != Caster)
            {
                // [핵심] USkillBase에 설계해두신 대로, 여기서 직접 데미지를 주지 않고 방송만 합니다.
                // 이 방송(OnSkillHit)을 CombatHandler가 듣고, SkillEffect를 생성하여 적에게 부착/적용하게 됩니다.
                ProcessHit(HitActor, FinalDamage);
                
                UE_LOG(LogSkill, Display, TEXT("%s가 %s에게 %f 데미지 판정 성공!"), *SkillName.ToString(), *HitActor->GetName(), FinalDamage);
            }
        }
    }

    // 허공에 썼든 적을 맞췄든 돌진 행위 자체는 발동되었으므로 true(성공) 반환
    return true; 
}