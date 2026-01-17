#include "Combat/ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(12.f);
	Collision->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = Collision;

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 2000.f;
	Movement->MaxSpeed = 2000.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;

	Collision->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::InitPayload(AActor* InSender, float InDamage, float InLifeSeconds)
{
	Sender = InSender;
	Damage = InDamage;

	if (InLifeSeconds > 0.f)
	{
		SetLifeSpan(InLifeSeconds);
	}
}

void AProjectileBase::OnHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor,
							UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/, const FHitResult& /*Hit*/)
{
	if (!OtherActor || OtherActor == Sender)
	{
		Destroy();
		return;
	}

	// 가상함수 인터페이스 호출: C++ 구현체만 대상이 됩니다.
	if (IHurtHandler* Hurt = Cast<IHurtHandler>(OtherActor))
	{
		FHurtRequest Req;
		Req.Sender = Sender;
		Req.Receiver = OtherActor;
		Req.Damage = Damage;

		Hurt->HandleHurt(Req);
	}

	Destroy();
}
