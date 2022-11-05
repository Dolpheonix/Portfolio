// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Giant_BT.h"
#include "Giant_Controller.generated.h"

/**
 * 
 */
UCLASS()
class QUICKSTART_API AGiant_Controller : public AAIController
{
	GENERATED_BODY()
	
public:
	AGiant_Controller();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoardKey")
	FName EnemyKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoardKey")
	FName OriginLocKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoardKey")
	FName OriginYawKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoardKey")
	FName HurtKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree")
	UBehaviorTree* BehaviorTree;
};
