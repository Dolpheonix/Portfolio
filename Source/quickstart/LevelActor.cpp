// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelActor.h"
#include <Kismet/GameplayStatics.h>

void ALevelActor::BeginPlay()
{
	Super::BeginPlay();

	auto playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	startLocation = playerPawn->GetActorLocation();
	startRotation = playerPawn->GetController()->GetControlRotation();
}

void ALevelActor::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	auto playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	float pawnHeight = playerPawn->GetActorLocation().Z;

	if (pawnHeight < deadPoint)
	{
		bIsDead = true;
	}

	if (bIsDead)
	{
		playerPawn->SetActorLocation(startLocation);
		playerPawn->GetController()->SetControlRotation(startRotation);
		bIsDead = false;
	}
}