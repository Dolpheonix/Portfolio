#include "MainCharacter.h"

void AMainCharacter::NoItem()
{
}

void AMainCharacter::ThrowApple()
{
	FVector ThrowPoint = GetActorLocation() + FVector(0.0f, 0.0f, 60.0f) + 15.0f * GetActorForwardVector();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AThrowable* Apple = GetWorld()->SpawnActor<AThrowable>(AThrowable::StaticClass(), ThrowPoint, FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
	if (Apple)
	{
		Apple->CollisionComponent->InitSphereRadius(13.0f);
		Apple->MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(Apple, TEXT("/Game/ShootingGame/Asset/Apple/Apple.Apple")));
		Apple->MeshComponent->SetRelativeScale3D(FVector(13.0f, 13.0f, 13.0f));
		Apple->MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -52.657f));
		FVector ThrowDirection = GetActorForwardVector();
		Apple->Throw(ThrowDirection);
	}
}