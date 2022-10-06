// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Root.h"

// Sets default values
ACharacter_Root::ACharacter_Root()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACharacter_Root::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacter_Root::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacter_Root::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
