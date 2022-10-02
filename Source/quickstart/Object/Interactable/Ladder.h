// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Ladder.generated.h"

/**
 * 
 */
UCLASS()
class QUICKSTART_API ALadder : public AInteractable
{
	GENERATED_BODY()

public:
	ALadder();

protected:
	virtual void BeginPlay() override;
	virtual void Interact() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LadderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Slope;

	float Width;

	FVector Left;
};
