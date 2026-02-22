#include "Mimic.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

AMimic::AMimic()
{
    // 기본 생성자. 부모 클래스에서 필요한 컴포넌트가 모두 생성되므로 비워둡니다.
}

void AMimic::BeginPlay()
{
    Super::BeginPlay();

    // 1. 초기 상태 설정: 이동 불가 처리
    if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent()))
    {
        MoveComp->SetMovementMode(MOVE_None); 
    }

    // 2. 체력바 UI 숨김 처리 (상태를 숨기기 위함)
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }

    // 3. 위장 플립북 적용
    if (DisguiseFlipbook && EnemyFlipbook) 
    {
        EnemyFlipbook->SetFlipbook(DisguiseFlipbook);
        EnemyFlipbook->SetLooping(true);
        EnemyFlipbook->Play();
    }
}

void AMimic::UpdateAnimation()
{
    // 위장 중이거나 깨어나는 연출 중일 때는 부모 클래스의 애니메이션(Idle/Forward) 갱신을 차단
    if (bIsDisguised || bIsRevealing)
    {
        return;
    }

    // 완전히 깨어난 후에는 일반 몬스터와 동일하게 애니메이션 업데이트 수행
    Super::UpdateAnimation();
}

void AMimic::Interact_Implementation(AActor* Interactor)
{
    if (!bIsDisguised) return;

    // 상호작용한 대상을 목표로 기상 시퀀스 시작
    WakeUp(Interactor);

    // 상호작용에 의한 확정 기습 데미지 연산
    if (Interactor)
    {
        FDamageEvent DamageEvent;
        Interactor->TakeDamage(SurpriseDamage, DamageEvent, GetController(), this);
    }
}

float AMimic::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 위장 상태에서 피격당했다면 위장 해제 (데미지 반환은 하지 않음)
    if (bIsDisguised)
    {
        WakeUp(DamageCauser);
    }

    // 부모 클래스의 정상적인 데미지 처리 수행
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AMimic::WakeUp(AActor* TargetActor)
{
    if (!bIsDisguised) return;

    bIsDisguised = false;
    bIsRevealing = true;

    // 1. 체력바 UI 활성화
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(true);
    }

    // 2. 청각적 피드백 제공
    if (WakeUpSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, WakeUpSound, GetActorLocation());
    }

    // 3. 시각적 상태 전환: 깨어나는 애니메이션 재생
    float RevealDuration = 0.5f; // 기본 대기 시간

    if (RevealFlipbook && EnemyFlipbook)
    {
        EnemyFlipbook->SetFlipbook(RevealFlipbook);
        EnemyFlipbook->SetLooping(false);
        EnemyFlipbook->PlayFromStart();
        RevealDuration = RevealFlipbook->GetTotalDuration();
    }

    // 4. 연출 종료 후 정상 몬스터 상태로 전환 예약
    GetWorld()->GetTimerManager().SetTimer(RevealTimer, this, &AMimic::FinishReveal, RevealDuration, false);
}

void AMimic::FinishReveal()
{
    bIsRevealing = false;

    // 이동 및 AI 추적을 위한 물리/운동 상태 복구
    if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent()))
    {
        MoveComp->SetMovementMode(MOVE_Walking);
    }

    // 플립북 루프 복원 및 부모 클래스의 Idle 애니메이션 호출 유도
    if (EnemyFlipbook)
    {
        EnemyFlipbook->SetLooping(true);
    }
}