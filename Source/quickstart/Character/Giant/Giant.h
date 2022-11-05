// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Character_Root.h"
#include "Giant_Controller.h"
#include <Engine/Classes/Components/SphereComponent.h>
#include "Giant.generated.h"

UCLASS()
class QUICKSTART_API AGiant : public ACharacter_Root
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGiant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void Attack(int elapsedFrame);

	USphereComponent* RockCollision;
	UStaticMeshComponent* Rock;
};
