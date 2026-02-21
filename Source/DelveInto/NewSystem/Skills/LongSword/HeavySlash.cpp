// Fill out your copyright notice in the Description page of Project Settings.


#include "HeavySlash.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NewSystem/Utils/CombatUtil.h"


// Sets default values for this component's properties
USkill_LongSword_HeavySlash::USkill_LongSword_HeavySlash()
{
	ExecutionType = ESkillExecutionType::Immediate; // 즉발
	bHasCombo = false;
    
	// 기본 트레이스 채널 설정 (블루프린트에서 변경 가능)
	TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
}


bool USkill_LongSword_HeavySlash::ExecuteSkillLogic_Implementation()
{
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return false;

    FVector EyeLocation;
    FRotator EyeRotation;
    OwnerChar->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    // 1. 앞으로 향하는 방향 벡터
    FVector ForwardVector = EyeRotation.Vector();
    
    // 2. [핵심] 바라보는 방향의 수직인 '오른쪽(Right)' 방향 벡터
    FVector RightVector = FRotationMatrix(EyeRotation).GetUnitAxis(EAxis::Y);

    // 3. 내 눈앞에 칼이 휘둘러질 '중심점'을 잡습니다.
    FVector SlashCenter = EyeLocation + (ForwardVector * ForwardDistance);

    // 4. 가로 베기의 시작점(왼쪽)과 끝점(오른쪽) 설정
    FVector StartLocation = SlashCenter - (RightVector * (SlashWidth * 0.5f));
    FVector EndLocation   = SlashCenter + (RightVector * (SlashWidth * 0.5f));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerChar);
    TArray<FHitResult> HitResults;

    EDrawDebugTrace::Type DebugDrawType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    // 5. 왼쪽에서 오른쪽으로 훑는 구체 트레이스 발사!
    bool bHitSomething = UKismetSystemLibrary::SphereTraceMulti(
        this,
        StartLocation, // 왼쪽에서
        EndLocation,   // 오른쪽으로 그음
        SlashRadius,   // 두께
        TraceChannel,
        false,
        ActorsToIgnore,
        DebugDrawType,
        HitResults,
        true
    );

    bool bSuccessfullyHitEnemy = false;

    if (bHitSomething)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor)
            {
                float FinalDamage = CalculateDamage();
                bool bDealtDamage = UCombatUtil::ApplyDamageIfEnemy(this, OwnerChar, HitActor, FinalDamage, OwnerChar);
                
                if (bDealtDamage)
                {
                    bSuccessfullyHitEnemy = true;
                }
            }
        }
    }

    return bSuccessfullyHitEnemy;
}
