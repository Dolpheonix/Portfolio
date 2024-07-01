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

	// 현재 상호작용 상태면 태스크 Fail 처리
	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == true)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UNpcTask_Idle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 상호작용 상태가 되면 태스크 실패
	const bool isInteracting = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(IsInteracting_Key.SelectedKeyName);
	if (isInteracting == true)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	// Idle Time이 지나면 태스크 성공
	static float elapsedTime = 0.f;
	elapsedTime += DeltaSeconds;	
	if (elapsedTime > IdleTime)
	{
		elapsedTime = 0.f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

