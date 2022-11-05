// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Chain.generated.h"

UCLASS()
class QUICKSTART_API AChain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when Property Changed in Editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void UpdateSegments();
	void UpdateBind(bool isLeft);
	void AdjustSegments();


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chain")
	TArray<UStaticMeshComponent*> Chains;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chain")
	TArray<UPhysicsConstraintComponent*> Joints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chain")
	UPhysicsConstraintComponent* L_Constraint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chain")
	UPhysicsConstraintComponent* R_Constraint;
	UPROPERTY(EditAnywhere, Category = "Chain")
	UStaticMesh* ChainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chain")
	int Segments = 1;

	int Segments_before = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chain")
	float Limit = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bind")
	AActor* L_BindActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bind")
	AActor* R_BindActor;
};
