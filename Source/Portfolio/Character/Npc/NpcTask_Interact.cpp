#include "NpcTask_Interact.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UNpcTask_Interact::UNpcTask_Interact()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	IsInteracting_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNpcTask_Interact, IsInteracting_Key));
	InteractTarget_Key.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UNpcTask_Interact, InteractTarget_Key), AActor::StaticClass());
}

EBTNodeResult::Type UNpcTask_Interact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == false)
	{
		return EBTNodeResult::Failed;
	}

	TObjectPtr<AAIController> ownerController = OwnerComp.GetAIOwner();

	ownerController->StopMovement();

	TObjectPtr<APawn> ownerActor = ownerController->GetPawn();
	TObjectPtr<AActor> targetActor = Cast<AActor>(ownerController->GetBlackboardComponent()->GetValueAsObject(InteractTarget_Key.SelectedKeyName));

	check(ownerActor);

	if (!targetActor)
	{
		return EBTNodeResult::Failed;	//TEMP : NpcCharacter::Interact() ȣ�� ��, isIntaracting ���� interacting target ������Ʈ�� ���������� ������Ʈ��. Ȥ�� �� �������� �߻� �� �������� �ٽ� ���Բ� ��.
	}

	const FVector lookVector = (targetActor->GetActorLocation() - ownerActor->GetActorLocation()).GetSafeNormal();
	const FRotator lookDirection = FRotationMatrix::MakeFromX(lookVector).Rotator();

	ownerActor->SetActorRotation(lookDirection);

	return EBTNodeResult::InProgress;
}

void UNpcTask_Interact::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}


