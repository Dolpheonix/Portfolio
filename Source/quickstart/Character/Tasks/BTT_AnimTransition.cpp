// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AnimTransition.h"

UBTT_AnimTransition::UBTT_AnimTransition()
{
	bNotifyTick = true;
	NodeName = "AnimTransition";
	bCreateNodeInstance = false;
}


EBTNodeResult::Type UBTT_AnimTransition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller = OwnerComp.GetAIOwner();
	ControlledPawn = (ACharacter_Root*)Controller->GetPawn();
	Blackboard = Controller->GetBlackboardComponent();

	ControlledPawn->GetMesh()->PlayAnimation(Anim_A, Loop_A);

	return EBTNodeResult::InProgress;
}

void UBTT_AnimTransition::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (Blackboard->GetValueAsBool(AtoBKey))
	{
		Blackboard->SetValueAsBool(AtoBKey, false);
		ControlledPawn->GetMesh()->PlayAnimation(Anim_B, Loop_B);
	}
	else if (Blackboard->GetValueAsBool(BtoAKey))
	{
		Blackboard->SetValueAsBool(BtoAKey, false);
		ControlledPawn->GetMesh()->PlayAnimation(Anim_A, Loop_A);
	}
}