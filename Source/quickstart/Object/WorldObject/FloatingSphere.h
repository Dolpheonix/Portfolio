// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Character/Main/MainCharacter.h"
#include <Engine/Classes/Components/SphereComponent.h>
#include "GameFramework/ProjectileMovementComponent.h"
#include "FloatingSphere.generated.h"

UCLASS(BlueprintType)
class QUICKSTART_API AFloatingSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingSphere();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);


	UFUNCTION(BlueprintCallable)
	void Roll(const FVector& direction);

	USphereComponent* CollisionComponent;

	UStaticMeshComponent* SphereMesh;

	UProjectileMovementComponent* MovementComponent;

	AMainCharacter* Player;
};
