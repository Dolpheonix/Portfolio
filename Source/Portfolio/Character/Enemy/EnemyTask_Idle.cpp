#include "EnemyTask_Idle.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../EnemyCharacter.h"

static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

UEnemyTask_Idle::UEnemyTask_Idle(const FObjectInitializer& objinit) : Super(objinit)
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	// EEnemyState 블랙보드 키만 선택 가능
	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_Idle, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
}

EBTNodeResult::Type UEnemyTask_Idle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Patrol 상태가 아니면 태스크 진입 불가
	EEnemyState Mode = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (Mode != EEnemyState::Patrol)
	{
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UEnemyTask_Idle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Patrol 상태에서 벗어나면 태스크 종료
	EEnemyState Mode = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (Mode != EEnemyState::Patrol)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

