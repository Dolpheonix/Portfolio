// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	Player = (AMainCharacter*)UGameplayStatics::GetPlayerPawn(this, 0);

	InteractPoint = GetActorLocation();
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector playerLoc = Player->GetActorLocation();

	FVector playerLook = Player->GetActorForwardVector();

	playerLook.Normalize();

	FVector Ray = InteractPoint - playerLoc;
	Ray.Normalize();

	if (!bWait && FVector::Dist(playerLoc, InteractPoint) < 200.0f && FVector::DotProduct(playerLook, Ray) > 0.8f)
	{
		bInteractable = true;
		Player->bInteractable = true;

		if (Player->bInteracted)
		{
			Interact();
		}
	}
	else
	{
		bInteractable = false;
		//Player->bInteractable = false;
	}
}

void AInteractable::Interact()
{

}

