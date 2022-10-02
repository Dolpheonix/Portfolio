// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interactable.h"
#include "../Item/Itemspace.h"
#include "CollectableItem.generated.h"

/*
 If player interact with this actor, actor destroyed. But class of this item is added to player's inventory.
 */
UCLASS()
class QUICKSTART_API ACollectableItem : public AInteractable
{
	GENERATED_BODY()
	
public:
	ACollectableItem();

	virtual void BeginPlay() override;
	virtual void Interact() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Type")
	bool isWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Type")
	EArmorySpace WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Type")
	EinventorySpace ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* AuraComponent;
};
