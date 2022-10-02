// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableItem.h"

ACollectableItem::ACollectableItem()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	AuraComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	AuraComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Aura(TEXT("/Game/FXVarietyPack/Particles/P_ky_healAura.P_ky_healAura"));
	if (Aura.Succeeded())
	{
		AuraComponent->SetTemplate(Aura.Object);
		AuraComponent->Deactivate();
	}
}

void ACollectableItem::BeginPlay()
{
	Super::BeginPlay();

	Forward = GetActorForwardVector();
}

void ACollectableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInteractable)
	{
		AuraComponent->Activate();
	}
	else
	{
		AuraComponent->Deactivate();
	}

	MeshComponent->AddRelativeRotation(FRotator(0.0f, 0.5f, 0.0f));
}

void ACollectableItem::Interact()
{
	if (isWeapon)
	{
		Player->Armory[(uint8)WeaponType] = true;
		Player->RefreshArmory();
		Player->GetWeapon((uint8)WeaponType);
	}
	else
	{
		Player->Inventory[(uint8)ItemType] = true;
		Player->RefreshInventory();
	}
	Destroy();
}