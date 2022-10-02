// Fill out your copyright notice in the Description page of Project Settings.


#include "CausticStuff.h"
#include "../../Character/MainCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACausticStuff::ACausticStuff()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACausticStuff::OnHit);
	CollisionComponent->InitSphereRadius(150.0f);
	RootComponent = CollisionComponent;
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshFinder(TEXT("/Game/ShootingGame/Asset/Sphere.Sphere"));
	if (meshFinder.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(meshFinder.Object);
		ProjectileMesh->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));
		ProjectileMesh->SetCollisionProfileName(TEXT("Projectile"));
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> materialFinder(TEXT("/Game/ShootingGame/Material/M_Metal_Steel.M_Metal_Steel"));
	if (materialFinder.Succeeded())
	{
		ProjectileMesh->SetMaterial(0, materialFinder.Object);
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->MaxSpeed = 1000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.2f;

	InitialLifeSpan = 6.0f;
}

// Called when the game starts or when spawned
void ACausticStuff::BeginPlay()
{
	Super::BeginPlay();

	if (!bGravity)
	{
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}
}

// Called every frame
void ACausticStuff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetActorLocation().Z < -600.0f)
	{
		Destroy();
	}
}

void ACausticStuff::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// 플레이어와 충돌했을 때 : 플레이어의 체력을 깎고 caustic stuff는 제거
	if (OtherActor->IsA(AMainCharacter::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("Ouchhhhhhhhhh!"));
		auto character = (AMainCharacter*)OtherActor;
		character->HP--;
		character->bHurt = true;
		Destroy();
	}
	// 다른 Actor와 충돌했을 때 : 충돌한 actor 제거
	else {
		OtherActor->Destroy();
	}
}

void ACausticStuff::ApplyGravity()
{
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
}
