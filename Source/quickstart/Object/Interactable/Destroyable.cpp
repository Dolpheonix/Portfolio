// Fill out your copyright notice in the Description page of Project Settings.


#include "Destroyable.h"

ADestroyable::ADestroyable()
{
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ADestroyable::BeginPlay()
{
	Super::BeginPlay();

	Forward = GetActorForwardVector();
}

void ADestroyable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInteractable)
	{
		MeshComponent->SetRenderCustomDepth(true);
	}
	else
	{
		MeshComponent->SetRenderCustomDepth(false);
	}
}

void ADestroyable::Interact()
{
	Destroy();
}

