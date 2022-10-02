// Fill out your copyright notice in the Description page of Project Settings.


#include "Elevator.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AElevator::AElevator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ElevatorBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorBody"));
	ElevatorBody->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> bodyMesh(TEXT("/Game/ShootingGame/Asset/Elevator/Elevator_Body.Elevator_Body"));
	if (bodyMesh.Succeeded())
	{
		ElevatorBody->SetStaticMesh(bodyMesh.Object);
		ElevatorBody->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}

	ElevatorInternalSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorInternalSwitch"));
	ElevatorInternalSwitch->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> internalSwitchMesh(TEXT("/Game/ShootingGame/Asset/Elevator/Switch.Switch"));
	if (internalSwitchMesh.Succeeded())
	{
		ElevatorInternalSwitch->SetStaticMesh(internalSwitchMesh.Object);
		ElevatorInternalSwitch->SetRelativeLocation(FVector(80.0f, 15.0f, 150.0f));
		ElevatorInternalSwitch->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		ElevatorInternalSwitch->SetRelativeScale3D(FVector(0.7f, 0.7f, 0.7f));
	}

	ElevatorExternalSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorExternalSwitch"));
	ElevatorExternalSwitch->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> externalSwitchMesh(TEXT("/Game/ShootingGame/Asset/Elevator/Switch.Switch"));
	if (externalSwitchMesh.Succeeded())
	{
		ElevatorExternalSwitch->SetStaticMesh(externalSwitchMesh.Object);
		ElevatorExternalSwitch->SetRelativeLocation(FVector(80.0f, -15.0f, 150.0f));
		ElevatorExternalSwitch->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		ElevatorExternalSwitch->SetRelativeScale3D(FVector(0.7f, 0.7f, 0.7f));
	}

	ElevatorDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorDoor"));
	ElevatorDoor->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> doorMesh(TEXT("/Game/ShootingGame/Asset/Elevator/Elevator_Door.Elevator_Door"));
	if (doorMesh.Succeeded())
	{
		ElevatorDoor->SetStaticMesh(doorMesh.Object);
		ElevatorDoor->SetRelativeLocation(FVector(140.0f, 0.0f, 0.0f));
	}

	ElevatorCeiling = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorCeiling"));
	ElevatorCeiling->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ceilingMesh(TEXT("/Game/ShootingGame/Architecture/Floor_400x400.Floor_400x400"));
	if (ceilingMesh.Succeeded())
	{
		ElevatorCeiling->SetStaticMesh(ceilingMesh.Object);
		ElevatorCeiling->SetRelativeLocation(FVector(0.0f, 10.0f, 420.0f));
	}

	ElevatorLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ElevatorLight"));
	ElevatorLight->SetupAttachment(RootComponent);
	ElevatorLight->SetRelativeLocation(FVector(200.0f, 200.0f, 380.0f));
	ElevatorLight->SetIntensity(8.0f);
	ElevatorLight->SetAttenuationRadius(1000.0f);
	ElevatorLight->SetMobility(EComponentMobility::Movable);

}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();

	InteractPoint += ElevatorExternalSwitch->GetRelativeLocation();
	Forward = -ElevatorExternalSwitch->GetForwardVector();

	HeightStamp = GetActorLocation().Z;
}

// Called every frame
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector playerLocal = UKismetMathLibrary::InverseTransformLocation(GetTransform(), Player->GetActorLocation());

	if (!bInside && playerLocal.Y > 10.0f)
	{
		bInside = true;
		InteractPoint = GetActorLocation() + ElevatorInternalSwitch->GetRelativeLocation();
		Forward = -Forward;
	}
	else if(bInside && playerLocal.Y < -10.0f)
	{
		bInside = false;
		InteractPoint = GetActorLocation() + ElevatorExternalSwitch->GetRelativeLocation();
		Forward = -Forward;
	}

	if (bElevating)
	{
		if (HeightStamp + Distance > GetActorLocation().Z)
		{
			AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
		}
		else
		{
			bElevating = false;
			bElevated = true;
			HeightStamp = GetActorLocation().Z;

			bOpening = true;
		}
	}
	else if (bDropping)
	{
		if (HeightStamp - Distance < GetActorLocation().Z)
		{
			AddActorWorldOffset(FVector(0.0f, 0.0f, -1.0f));
		}
		else
		{
			bDropping = false;
			bElevated = false;
			HeightStamp = GetActorLocation().Z;

			bOpening = true;
		}
	}
	else if (bOpening)
	{
		if (ElevatorDoor->GetRelativeLocation().X > 10.0f)
		{
			ElevatorDoor->AddRelativeLocation(FVector(-1.0f, 0.0f, 0.0f));
		}
		else if (OpenedTime < 3.0f)
		{
			OpenedTime += DeltaTime;
		}
		else
		{
			OpenedTime = 0.0f;
			bOpening = false;
			bClosing = true;
		}
	}
	else if (bClosing)
	{
		if (ElevatorDoor->GetRelativeLocation().X < 140.0f)
		{
			ElevatorDoor->AddRelativeLocation(FVector(1.0f, 0.0f, 0.0f));
		}
		else
		{
			bClosing = false;
			bWait = false;
		}
	}

	if (bInteractable)
	{
		if (bInside)
		{
			ElevatorInternalSwitch->SetRenderCustomDepth(true);
			ElevatorExternalSwitch->SetRenderCustomDepth(false);
		}
		else
		{
			ElevatorExternalSwitch->SetRenderCustomDepth(true);
			ElevatorInternalSwitch->SetRenderCustomDepth(false);
		}
	}
	else
	{
		ElevatorExternalSwitch->SetRenderCustomDepth(false);
		ElevatorInternalSwitch->SetRenderCustomDepth(false);
	}

}

void AElevator::Interact()
{
	if (!bInside)
	{
		bWait = true;
		bOpening = true;
	}
	else // Elevator 내부에선 문이 완전히 닫힌 상태에서만 조작이 가능
	{
		if (bElevated)
		{
			bWait = true;
			bDropping = true;
		}
		else
		{
			bWait = true;
			bElevating = true;
		}
	}
}

