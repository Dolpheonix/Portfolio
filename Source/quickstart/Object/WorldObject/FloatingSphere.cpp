// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingSphere.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AFloatingSphere::AFloatingSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFloatingSphere::OnHit);
	CollisionComponent->InitSphereRadius(150.0f);
	RootComponent = CollisionComponent;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SphereMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshFinder(TEXT("/Game/ShootingGame/Asset/Sphere.Sphere"));
	if (meshFinder.Succeeded())
	{
		SphereMesh->SetStaticMesh(meshFinder.Object);
		SphereMesh->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));
		SphereMesh->SetCollisionProfileName(TEXT("Default"));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> materialFinder(TEXT("/Game/ShootingGame/Material/M_Metal_Steel.M_Metal_Steel"));
	if (materialFinder.Succeeded())
	{
		SphereMesh->SetMaterial(0, materialFinder.Object);
	}

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);
	MovementComponent->InitialSpeed = 0.0f;
	MovementComponent->MaxSpeed = 1000.0f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bShouldBounce = true;
	MovementComponent->Bounciness = 0.5f;
	MovementComponent->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 1000.0f;
}

// Called when the game starts or when spawned
void AFloatingSphere::BeginPlay()
{
	Super::BeginPlay();
	
	Player = (AMainCharacter*)UGameplayStatics::GetPlayerPawn(this, 0);
}

// Called every frame
void AFloatingSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFloatingSphere::Roll(const FVector& direction)
{
	MovementComponent->Velocity = direction * 3000.0f;
}

void AFloatingSphere::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this)
	{
		Destroy();
		if (OtherActor->IsA(AMainCharacter::StaticClass()))
		{
			auto character = (AMainCharacter*)OtherActor;
			character->HP--;
			character->bHurt = true;
		}
		else
		{
			OtherActor->Destroy();
		}
	}
}
