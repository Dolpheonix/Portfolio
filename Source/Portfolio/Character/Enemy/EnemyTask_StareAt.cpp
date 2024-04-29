#include "EnemyTask_StareAt.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../EnemyCharacter.h"

// ExecuteTask(), TickTask()
static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

UEnemyTask_StareAt::UEnemyTask_StareAt()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	// EEnemyState 블랙보드 키만 선택 가능
	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_StareAt, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
}

EBTNodeResult::Type UEnemyTask_StareAt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Caution 상태가 아니면 태스크 진입 불가
	const EEnemyState currState = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Caution) 
	{
		return EBTNodeResult::Failed;
	}

	// 진행중인 이동 명령 중지
	OwnerComp.GetAIOwner()->StopMovement();

	return EBTNodeResult::InProgress;
}

void UEnemyTask_StareAt::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Caution 상태에서 벗어나면 태스크 종료 처리
	const EEnemyState currState = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Caution)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
