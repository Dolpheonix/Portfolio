#include "EnemyTask_Chase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../EnemyCharacter.h"

static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

UEnemyTask_Chase::UEnemyTask_Chase(const FObjectInitializer& objinit) : Super(objinit)
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	// Enemy State 블랙보드 키만 선택할 수 있음.
	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_Chase, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
}

EBTNodeResult::Type UEnemyTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Detected 상태가 아니면 태스크 진입 불가
	const EEnemyState currState = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Detected) 
	{
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);;
}

void UEnemyTask_Chase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Detected 상태에서 벗어나면 태스크 실패 처리
	const EEnemyState currState = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Detected)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
