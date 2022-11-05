// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "../Character_Root.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimSequence.h"
#include "BTT_AnimTransition.generated.h"

/**
 * 
 */
UCLASS()
class QUICKSTART_API UBTT_AnimTransition : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_AnimTransition();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Key")
	FName AtoBKey;
	UPROPERTY(EditAnywhere, Category = "Key")
	FName BtoAKey;

	UPROPERTY(EditAnywhere, Category = "A")
	UAnimSequence* Anim_A;
	UPROPERTY(EditAnywhere, Category = "A")
	bool Loop_A;

	UPROPERTY(EditAnywhere, Category = "B")
	UAnimSequence* Anim_B;
	UPROPERTY(EditAnywhere, Category = "B")
	bool Loop_B;

private:
	ACharacter_Root* ControlledPawn;
	AAIController* Controller;
	UBlackboardComponent* Blackboard;
};
