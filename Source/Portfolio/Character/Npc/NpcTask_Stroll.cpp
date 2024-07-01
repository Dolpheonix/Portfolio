#include "NpcTask_Stroll.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UNpcTask_Stroll::UNpcTask_Stroll(const FObjectInitializer& objinit) : Super(objinit)
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	IsInteracting_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNpcTask_Stroll, IsInteracting_Key));
}

EBTNodeResult::Type UNpcTask_Stroll::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 상호작용 상태면 태스크 Fail 처리
	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == true)
	{
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UNpcTask_Stroll::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	// 상호작용 상태가 되면 태스크 종료
	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == true)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}


