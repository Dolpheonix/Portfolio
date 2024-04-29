#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/CustomCharacter.h"

ABullet::ABullet()
{
	mCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	mCollisionComponent->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	mCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapped);
	mCollisionComponent->InitSphereRadius(3.f);
	mCollisionComponent->SetCollisionProfileName(TEXT("Bullet"));
	RootComponent = mCollisionComponent;

	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mMeshComponent->SetupAttachment(RootComponent);

	mBulletMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	mBulletMovementComponent->InitialSpeed = 3000.f;
	mBulletMovementComponent->MaxSpeed = 3000.f;
	mBulletMovementComponent->ProjectileGravityScale = 0.f;
	mBulletMovementComponent->bRotationFollowsVelocity = false;
	mBulletMovementComponent->bShouldBounce = false;
	mBulletMovementComponent->SetUpdatedComponent(RootComponent);

	Speed = mBulletMovementComponent->InitialSpeed;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLifeSpan = 6000.f / Speed;
}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		// ��Ʈ �̺�Ʈ �ÿ��� ���� ������Ʈ�� ƨ���� ���� (������ ���� �ÿ���)
		OtherComponent->AddImpulseAtLocation(mBulletMovementComponent->Velocity * 100.f, Hit.ImpactPoint);
	}

	// ��Ʈ �� ���ʹ� ����
	Destroy();
}

void ABullet::OnOverlapped(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TObjectPtr<ACustomCharacter> Hittee = Cast<ACustomCharacter>(OtherActor);
	if (Hittee)
	{
		// ������ ����
		UGameplayStatics::ApplyDamage(Hittee, mDamage, nullptr, this, NULL);
	}

	// ���� ����
	Destroy();
}

void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FString propName = PropertyChangedEvent.Property->GetName();
	if (propName == "Speed")
	{
		// Speed Parameter ���� ��, �����Ʈ ������Ʈ�� �ӵ� �� ����
		mBulletMovementComponent->InitialSpeed = Speed;
		mBulletMovementComponent->MaxSpeed = Speed;
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ABullet::Launch(const FVector& direction, float damage)
{
	mDamage = damage;
	mBulletMovementComponent->Velocity = direction * mBulletMovementComponent->InitialSpeed;
}