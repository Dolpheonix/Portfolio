// Fill out your copyright notice in the Description page of Project Settings.


#include "Ladder.h"
#include <Kismet/KismetMathLibrary.h>


ALadder::ALadder()
{
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Game/ShootingGame/Asset/Ladder.Ladder"));
	if (Mesh.Succeeded())
	{
		LadderMesh->SetStaticMesh(Mesh.Object);
	}
}

void ALadder::BeginPlay()
{
	Super::BeginPlay();

	Forward = GetActorRightVector();

	Left = GetActorForwardVector();

	Slope = GetActorRotation().Roll + 75.0f;

	Width = GetActorScale3D().X * 50.0f;
}

void ALadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInteractable)
	{
		LadderMesh->SetRenderCustomDepth(true);
	}
	else
	{
		LadderMesh->SetRenderCustomDepth(false);
	}
}

void ALadder::Interact()
{
	FVector middlePoint = GetActorLocation() - 0.5f * Width * Left + FVector(0.0f, 0.0f, 88.0f);
	FVector forcePoint = middlePoint + 30.0f * Forward;
	FVector Look = middlePoint - forcePoint;
	Look.Normalize();
	float YawDeg = UKismetMathLibrary::Acos(FVector::DotProduct(Look, Player->GetActorForwardVector()));
	float Yaw = UKismetMathLibrary::RadiansToDegrees(YawDeg);

	Player->SetActorLocation(forcePoint);
	Player->GetController()->SetControlRotation(UKismetMathLibrary::MakeRotFromX(-Forward));

	Player->LadderInfo.onLadder = true;
	Player->LadderInfo.Slope = Slope;
	Player->LadderInfo.Width = Width;
	Player->LadderInfo.dirty = true;
}