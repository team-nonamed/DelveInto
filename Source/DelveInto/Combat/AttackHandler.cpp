#include "Combat/AttackHandler.h"

#include "Skills/Handlers/AbstractSkillHandler.h"
#include "Items/InventoryHandler.h"
#include "Items/ItemInstance.h"
#include "Skills/Instances/SkillInstance.h"
#include "Skills/Data/SkillData.h"

#include "Combat/CombatTargetingLibrary.h"
#include "Combat/ProjectileBase.h"
#include "Interfaces/Hurts/HurtHandler.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entities/AbstractHost.h"
#include "GameFramework/ProjectileMovementComponent.h"

UAttackHandler::UAttackHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttackHandler::BeginPlay()
{
	Super::BeginPlay();

	// Host 패턴: 동일 Actor에 붙어있는 다른 핸들러를 찾아 연결
	SkillHandler = GetOwner() ? GetOwner()->FindComponentByClass<UAbstractSkillHandler>() : nullptr;
	InventoryHandler = GetOwner() ? GetOwner()->FindComponentByClass<UInventoryHandler>() : nullptr;
}

bool UAttackHandler::HandleInput(ESkillSlot Designator, ETriggerEvent TriggerEvent)
{
	USkillInstance* Skill = nullptr;

	if (Designator == ESkillSlot::BaseAttack || Designator == ESkillSlot::AltAttack)
	{
		if (InventoryHandler)
		{
			if (UItemInstance* Weapon = InventoryHandler->GetActivatedWeapon())
			{
				// 기본 공격은 무기가 제공하는 SkillInstance를 사용
				Skill = Weapon->GetBasicAttackSkillInst();
			}
		}
	}
	else
	{
		if (SkillHandler)
		{
			Skill = SkillHandler->GetSkill(Designator);
		}
	}

	return TryActivateSkill(Designator, Skill, TriggerEvent);
}

bool UAttackHandler::TryActivateSkill(ESkillSlot Designator, USkillInstance* Skill, ETriggerEvent TriggerEvent)
{
	if (!Skill || !GetWorld())
	{
		return false;
	}

	const FSkillContext Ctx = BuildContext();

	// 1) 입력 이벤트 → 상태 전이
	switch (TriggerEvent)
	{
	case ETriggerEvent::Started:
		Skill->OnInputStarted(GetWorld(), Ctx);
		break;
	case ETriggerEvent::Completed:
		Skill->OnInputCompleted(GetWorld(), Ctx);
		break;
	case ETriggerEvent::Canceled:
		Skill->OnInputCanceled(GetWorld(), Ctx);
		break;
	default:
		// Triggered/Ongoing 등은 필요 시 확장 (차지 UI 업데이트 등)
		break;
	}

	// 2) 발동 가능 여부
	if (!Skill->CanActivate(GetWorld(), Ctx))
	{
		return false;
	}

	const USkillData* Data = Skill->GetData();
	if (!Data)
	{
		return false;
	}

	const FSkillExecSpec& Exec = Data->GetExec();

	// 3) 실행(월드 액션)
	const bool bExecuted = ExecuteExecSpec(Designator, Skill, Exec, Ctx);
	if (!bExecuted)
	{
		return false;
	}

	// 4) 성공 커밋(쿨다운/콤보/차지 확정)
	Skill->CommitActivate(GetWorld());

	// 5) UI 바인딩용 쿨다운 이벤트(Started/Ended)
	if (Exec.CooldownSeconds > 0.f)
	{
		OnSkillCooldownStarted.Broadcast(Designator, Exec.CooldownSeconds);

		FTimerHandle& Handle = CooldownTimers.FindOrAdd(Designator);
		GetWorld()->GetTimerManager().ClearTimer(Handle);
		GetWorld()->GetTimerManager().SetTimer(
			Handle,
			FTimerDelegate::CreateUObject(this, &UAttackHandler::NotifyCooldownEnded, Designator),
			Exec.CooldownSeconds,
			false
		);
	}

	return true;
}

bool UAttackHandler::ExecuteExecSpec(ESkillSlot /*Designator*/, USkillInstance* Skill, const FSkillExecSpec& Exec, const FSkillContext& Ctx)
{
	const float Damage = Skill->ResolveDamage(GetWorld());

	switch (Exec.Delivery)
	{
	case ESkillDelivery::MeleeArea:
		return ExecuteMelee(Exec, Damage, Ctx);

	case ESkillDelivery::Projectile:
		return ExecuteProjectile(Exec, Damage, Ctx);

	case ESkillDelivery::SummonHost:
		return ExecuteSummon(Exec, Ctx);

	default:
		return false;
	}
}

bool UAttackHandler::ExecuteMelee(const FSkillExecSpec& Exec, float Damage, const FSkillContext& Ctx)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	// 충돌 채널/오브젝트 타입은 프로젝트 룰에 맞게 조정
	const FCollisionObjectQueryParams ObjParams(FCollisionObjectQueryParams::AllDynamicObjects);

	TArray<AActor*> Ignore;
	for (const TObjectPtr<AActor>& A : Ctx.ActorsToIgnore)
	{
		if (A) Ignore.Add(A);
	}

	TArray<AActor*> Targets;
	if (Exec.Melee.Shape == ESkillShape::Circle)
	{
		Targets = UCombatTargetingLibrary::FindActorsInCircle(
			World,
			Ctx.Origin,
			Exec.Melee.Radius,
			ObjParams,
			ECC_Visibility,
			Ignore,
			Exec.Damage.bRequireLOS,
			Exec.Melee.MaxTargets
		);
	}
	else
	{
		Targets = UCombatTargetingLibrary::FindActorsInCone(
			World,
			Ctx.Origin,
			Ctx.Forward,
			Exec.Melee.Radius,
			Exec.Melee.HalfAngleDeg,
			ObjParams,
			ECC_Visibility,
			Ignore,
			Exec.Damage.bRequireLOS,
			Ctx.bIgnoreZ,
			Exec.Melee.MaxTargets
		);
	}

	bool bAnyHit = false;

	// “가상함수 인터페이스” 호출이므로 C++ 구현체(주로 Host Actor)가 대상이어야 함
	for (AActor* Target : Targets)
	{
		if (!Target) continue;

		if (IHurtHandler* Hurt = Cast<IHurtHandler>(Target))
		{
			FHurtRequest Req;
			Req.Sender = Ctx.Instigator;
			Req.Receiver = Target;
			Req.Damage = Damage;

			Hurt->HandleHurt(Req);
			bAnyHit = true;
		}
	}

	// 정책: “헛스윙도 실행 성공으로 볼지”는 게임 디자인에 따라 결정
	return true;
}

bool UAttackHandler::ExecuteProjectile(const FSkillExecSpec& Exec, float Damage, const FSkillContext& Ctx)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	if (!Exec.Projectile.ProjectileClass)
	{
		return false;
	}

	FTransform SpawnTM;
	SpawnTM.SetLocation(Ctx.Origin);
	SpawnTM.SetRotation(Ctx.Forward.ToOrientationQuat());

	// 소켓이 지정되어 있고 Owner가 Character면 소켓 기반 스폰
	if (Exec.Projectile.MuzzleSocket != NAME_None)
	{
		if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
		{
			if (USkeletalMeshComponent* Mesh = Char->GetMesh())
			{
				if (Mesh->DoesSocketExist(Exec.Projectile.MuzzleSocket))
				{
					SpawnTM = Mesh->GetSocketTransform(Exec.Projectile.MuzzleSocket);
				}
			}
		}
	}

	AProjectileBase* Proj = World->SpawnActorDeferred<AProjectileBase>(
		Exec.Projectile.ProjectileClass,
		SpawnTM,
		GetOwner(),
		Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (!Proj) return false;

	Proj->InitPayload(Ctx.Instigator, Damage, Exec.Projectile.LifeSeconds);

	// 속도 적용
	if (UProjectileMovementComponent* Move = Proj->FindComponentByClass<UProjectileMovementComponent>())
	{
		Move->InitialSpeed = Exec.Projectile.InitialSpeed;
		Move->MaxSpeed = Exec.Projectile.InitialSpeed;
		Move->Velocity = SpawnTM.GetRotation().GetForwardVector() * Exec.Projectile.InitialSpeed;
	}

	Proj->FinishSpawning(SpawnTM);
	return true;
}

bool UAttackHandler::ExecuteSummon(const FSkillExecSpec& Exec, const FSkillContext& Ctx)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	if (!Exec.Summon.SummonedHostClass)
	{
		return false;
	}

	const FVector SpawnLoc = Ctx.Origin + Exec.Summon.SpawnOffset;
	const FRotator SpawnRot = Ctx.Forward.Rotation();

	World->SpawnActor<AActor>(
		Exec.Summon.SummonedHostClass,
		SpawnLoc,
		SpawnRot
	);

	return true;
}

void UAttackHandler::NotifyCooldownEnded(ESkillSlot Designator)
{
	OnSkillCooldownEnded.Broadcast(Designator);
}

FSkillContext UAttackHandler::BuildContext() const
{
	FSkillContext Ctx;

	Ctx.Instigator = GetOwner();

	if (const AActor* Owner = GetOwner())
	{
		Ctx.Origin = Owner->GetActorLocation();
		Ctx.Forward = Owner->GetActorForwardVector();
		Ctx.ActorsToIgnore.Add(const_cast<AActor*>(Owner));
	}

	return Ctx;
}
