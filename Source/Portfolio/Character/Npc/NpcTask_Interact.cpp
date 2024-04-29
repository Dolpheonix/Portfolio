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
		return EBTNodeResult::Failed;	//TEMP : NpcCharacter::Interact() 호출 시, isIntaracting 값과 interacting target 오브젝트가 순차적으로 업데이트됨. 혹시 모를 널포인터 발생 시 시퀀스를 다시 돌게끔 함.
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


