// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Components/PointLightComponent.h"
#include "Elevator.generated.h"

UCLASS()
class QUICKSTART_API AElevator : public AInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Interact() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ElevatorBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ElevatorInternalSwitch; // Control Elevator

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ElevatorExternalSwitch; // Open Elevator Door

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ElevatorDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ElevatorCeiling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPointLightComponent* ElevatorLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Distance; // Elevator의 상승/하강 거리
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bElevated; // Where is Relative Position? (Upper/Lower floor)

	float HeightStamp; // 시작점

	bool bInside = false; // Is Player in Elevator?

	bool bElevating;
	
	bool bDropping;

	bool bOpening;

	bool bClosing;

	bool bGetin;

	bool bGetout;

	float OpenedTime;
};
