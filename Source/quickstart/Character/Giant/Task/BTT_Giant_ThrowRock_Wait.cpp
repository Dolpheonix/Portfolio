// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Giant_ThrowRock_Wait.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../Object/WorldObject/CausticStuff.h"
#include "../../../Utils/MathUtil.h"

UBTT_Giant_ThrowRock_Wait::UBTT_Giant_ThrowRock_Wait()
{
	bNotifyTick = true;
	NodeName = "ThrowRock Giant Wait";
	bCreateNodeInstance = false;
}

EBTNodeResult::Type UBTT_Giant_ThrowRock_Wait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller = OwnerComp.GetAIOwner();
	ControlledPawn = (ACharacter_Root*) Controller->GetPawn();
	Blackboard = Controller->GetBlackboardComponent();

	ACausticStuff* rock;
	FActorSpawnParameters param;
	param.Owner = ControlledPawn;		
	param.Instigator = ControlledPawn;
	rock = GetWorld()->SpawnActor<ACausticStuff>(ACausticStuff::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), param);
	rock->AttachToComponent(ControlledPawn->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("hand_r_GiantBall"));
	Blackboard->SetValueAsObject(RockKey, rock);

	Enemy = (ACharacter_Root*) Blackboard->GetValueAsObject(EnemyKey);
	ElapsedTime = 0.0f;

	return EBTNodeResult::InProgress;
}

void UBTT_Giant_ThrowRock_Wait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	if (ElapsedTime > WaitTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	FVector giantForward = ControlledPawn->GetActorForwardVector();
	FVector desiredForward = Enemy->GetActorLocation() - ControlledPawn->GetActorLocation();
	desiredForward = FVector(desiredForward.X, desiredForward.Y, 0.f);
	desiredForward.Normalize();

	float angle = MathUtil::AngleBetweenTwoVectors_2D(giantForward, desiredForward);

	float origin = Controller->GetControlRotation().Yaw;
	Controller->SetControlRotation(FRotator(0.0f, origin + angle, 0.0f));
}
