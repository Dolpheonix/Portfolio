// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 발사체 collision 관리
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABullet::OnHit); // component에 hit event가 발생했을 때, OnHit function을 호출하도록 binding
	CollisionComponent->InitSphereRadius(15.0f);
	RootComponent = CollisionComponent;

	// 발사체 mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshFinder(TEXT("/Game/ShootingGame/Asset/Sphere.Sphere"));
	if (meshFinder.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(meshFinder.Object);
		ProjectileMesh->SetRelativeScale3D(FVector(0.02f, 0.02f, 0.02f));
		ProjectileMesh->SetCollisionProfileName(TEXT("Projectile"));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> materialFinder(TEXT("/Game/ShootingGame/Material/M_Metal_Steel.M_Metal_Steel"));
	if (materialFinder.Succeeded())
	{
		ProjectileMesh->SetMaterial(0, materialFinder.Object);
	}

	// 발사체 movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true; // velocity의 방향에 따라 발사체가 회전하게 됨
	ProjectileMovementComponent->bShouldBounce = true; // 발사체에 bounce를 적용
	ProjectileMovementComponent->Bounciness = 0.3f; // bounce 후에 보존되는 velocity

	// 발사체 지속 시간
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// HitComponent : 이 Actor 내에서 collision이 발생한 component (여기선 spherecomponent)
// OtherActor : collision이 발생한 반대쪽 Actor
// OtherComponent : collision이 발생한 반대쪽 component
// NormalImpulse : impulse vector 
void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics()) // other object가 physics를 실행하는 경우, hitting result를 적용해야 함
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}
}

// projectile의 velocity를 결정
void ABullet::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
