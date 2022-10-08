// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character_Root.generated.h"

UCLASS()
class QUICKSTART_API ACharacter_Root : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_Root();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float HP = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	bool bHurt = false;

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	bool bAttacking = false;

	// hurt event가 발생하면 경직시간이 필요하므로, hurt animation의 지속시간만큼 경직을 준다.
	int hurtFrameStep = -1;
};
