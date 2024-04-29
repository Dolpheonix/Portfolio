#include "NpcTask_Idle.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UNpcTask_Idle::UNpcTask_Idle() : IdleTime(0.f)
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	IsInteracting_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNpcTask_Idle, IsInteracting_Key));
}

EBTNodeResult::Type UNpcTask_Idle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	check(IdleTime > 0.f);

	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == true)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UNpcTask_Idle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == true)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	static float elapsedTime = 0.f;
	elapsedTime += DeltaSeconds;	
	if (elapsedTime > IdleTime)
	{
		elapsedTime = 0.f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

