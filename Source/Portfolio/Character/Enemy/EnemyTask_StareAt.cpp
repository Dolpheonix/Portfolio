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

	// EEnemyState ������ Ű�� ���� ����
	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_StareAt, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
}

EBTNodeResult::Type UEnemyTask_StareAt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Caution ���°� �ƴϸ� �½�ũ ���� �Ұ�
	const EEnemyState currState = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Caution) 
	{
		return EBTNodeResult::Failed;
	}

	// �������� �̵� ��� ����
	OwnerComp.GetAIOwner()->StopMovement();

	return EBTNodeResult::InProgress;
}

void UEnemyTask_StareAt::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Caution ���¿��� ����� �½�ũ ���� ó��
	const EEnemyState currState = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Caution)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
