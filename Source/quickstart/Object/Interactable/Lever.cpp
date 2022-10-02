// Fill out your copyright notice in the Description page of Project Settings.


#include "Lever.h"

ALever::ALever()
{
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LeverCase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverCase"));
	LeverCase->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> caseFinder(TEXT("/Game/ShootingGame/Asset/Lever/Lever_Case.Lever_Case"));
	if (caseFinder.Succeeded())
	{
		LeverCase->SetStaticMesh(caseFinder.Object);
	}

	LeverBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverBody"));
	LeverBody->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> bodyFinder(TEXT("/Game/ShootingGame/Asset/Lever/Lever_Body.Lever_Body"));
	if (bodyFinder.Succeeded())
	{
		LeverBody->SetStaticMesh(bodyFinder.Object);
		LeverBody->SetRelativeScale3D(FVector(1.5f, 1.0f, 1.0f));
		LeverBody->SetRelativeLocation(FVector(10.0f, 0.0f, 10.0f));
	}
}

void ALever::BeginPlay()
{
	Super::BeginPlay();

	LeverBody->SetRelativeRotation(FRotator(40.0f, 0.0f, 0.0f));
	Forward = GetActorForwardVector();
}

void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLeverDown)
	{
		if (LeverBody->GetRelativeRotation().Pitch <= -40.0f)
		{
			bLeverDown = false;
		}
		else
		{
			LeverBody->AddRelativeRotation(FRotator(-1.0f, 0.0f, 0.0f));
		}
	}
	else if (bWait)
	{
		if (LeverBody->GetRelativeRotation().Pitch >= 40.0f)
		{
			bWait = false;
		}
		else
		{
			LeverBody->AddRelativeRotation(FRotator(1.0f, 0.0f, 0.0f));
		}
	}

	if (bInteractable)
	{
		LeverBody->SetRenderCustomDepth(true);
		LeverCase->SetRenderCustomDepth(true);
	}
	else
	{
		LeverBody->SetRenderCustomDepth(false);
		LeverCase->SetRenderCustomDepth(false);
	}
}

void ALever::Interact()
{
	bLeverDown = true;
	bWait = true;
	Lever_On();
}