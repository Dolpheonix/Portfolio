// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "../../Character_Root.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTT_Giant_ThrowRock_Attack.generated.h"

/**
 * 
 */
UCLASS()
class QUICKSTART_API UBTT_Giant_ThrowRock_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_Giant_ThrowRock_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	float ElapsedTime;
	bool bReadyThrow;

	FName RockKey = TEXT("Rock");
	FName EnemyKey = TEXT("Enemy");
	
	UPROPERTY(EditAnywhere)
	float Speed;

	ACharacter_Root* Enemy;
	ACharacter_Root* ControlledPawn;
	AAIController* Controller;
	UBlackboardComponent* Blackboard;
};
