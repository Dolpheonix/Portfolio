// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Character/Main/MainCharacter.h"
#include "Interactable.generated.h"

UCLASS(Abstract)
class QUICKSTART_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Interact();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	AMainCharacter* Player;

	FVector InteractPoint;

	// Look vector of Interaction point
	FVector Forward;

	bool bInteractable = false;

	bool bWait = false;
};
