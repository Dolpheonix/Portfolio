// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Giant_ThrowRock_Attack.h"
#include "../../../Object/WorldObject/CausticStuff.h"
#include "../../../Utils/MathUtil.h"

UBTT_Giant_ThrowRock_Attack::UBTT_Giant_ThrowRock_Attack()
{
	bNotifyTick = true;
	NodeName = "ThrowRock Giant Attack";
	bCreateNodeInstance = false;
}

EBTNodeResult::Type UBTT_Giant_ThrowRock_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ElapsedTime = 0.0f;
	bReadyThrow = true;

	Controller = OwnerComp.GetAIOwner();
	ControlledPawn = (ACharacter_Root*) Controller->GetPawn();
	Blackboard = Controller->GetBlackboardComponent();
	Enemy = (ACharacter_Root*) Blackboard->GetValueAsObject(EnemyKey);

	return EBTNodeResult::InProgress;
}

void UBTT_Giant_ThrowRock_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	if (ElapsedTime < 1.5f)
	{
		FVector giantForward = ControlledPawn->GetActorForwardVector();
		FVector desiredForward = Enemy->GetActorLocation() - ControlledPawn->GetActorLocation();
		desiredForward = FVector(desiredForward.X, desiredForward.Y, 0.f);
		desiredForward.Normalize();

		float angle = MathUtil::AngleBetweenTwoVectors_2D(giantForward, desiredForward);

		float origin = Controller->GetControlRotation().Yaw;
		Controller->SetControlRotation(FRotator(0.0f, origin + angle, 0.0f));
	}
	else if (ElapsedTime > 1.5f && bReadyThrow)
	{
		auto rock = (ACausticStuff*) Blackboard->GetValueAsObject(RockKey);
		if (rock)
		{
			FDetachmentTransformRules detachRule(EDetachmentRule::KeepWorld, true);

			rock->DetachFromActor(detachRule);
			auto throwDir = Enemy->GetActorLocation() - rock->GetActorLocation();
			throwDir.Normalize();
			rock->ApplyGravity();
			rock->Throw(throwDir, Speed);
			bReadyThrow = false;
		}
	}
	
	if (ElapsedTime > 2.5f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

