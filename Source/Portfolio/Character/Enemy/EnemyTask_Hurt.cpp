#include "EnemyTask_Hurt.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "../EnemyCharacter.h"
#include "EnemyController.h"

static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

UEnemyTask_Hurt::UEnemyTask_Hurt() : mOwnerEnemy(nullptr)
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	// EEnemyState ������ Ű�� ���� ����
	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_Hurt, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
	CachedStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_Hurt, CachedStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
}

EBTNodeResult::Type UEnemyTask_Hurt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	mOwnerEnemy = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	check(mOwnerEnemy);

	TObjectPtr<UBlackboardComponent> blackboard = OwnerComp.GetAIOwner()->GetBlackboardComponent();

	// Hurt ���°� �ƴϸ� �½�ũ�� ������ �� ����.
	const EEnemyState currState = static_cast<EEnemyState>(blackboard->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Hurt)
	{
		return EBTNodeResult::Failed;
	}

	// ���� �������� �̵� ����� ���� 
	OwnerComp.GetAIOwner()->StopMovement();

	return EBTNodeResult::InProgress;
}

void UEnemyTask_Hurt::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// �ִϸ��̼��� ������ �½�ũ ����
	const bool isPlaying = mOwnerEnemy->GetMesh()->GetSingleNodeInstance()->IsPlaying();
	if (isPlaying == false)
	{
		const uint8 cached = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(CachedStateKey.SelectedKeyName);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsEnum(EnemyStateKey.SelectedKeyName, cached);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
