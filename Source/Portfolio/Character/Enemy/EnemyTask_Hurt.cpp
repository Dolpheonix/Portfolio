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

	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_Hurt, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
	CachedStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UEnemyTask_Hurt, CachedStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
}

EBTNodeResult::Type UEnemyTask_Hurt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	mOwnerEnemy = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	check(mOwnerEnemy);

	TObjectPtr<UBlackboardComponent> blackboard = OwnerComp.GetAIOwner()->GetBlackboardComponent();

	// Hurt 상태가 아니면 태스크 Fail 처리
	const EEnemyState currState = static_cast<EEnemyState>(blackboard->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (currState != EEnemyState::Hurt)
	{
		return EBTNodeResult::Failed;
	}

	// 현재 예약된 Movement 종료
	OwnerComp.GetAIOwner()->StopMovement();

	return EBTNodeResult::InProgress;
}

void UEnemyTask_Hurt::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 애니메이션이 끝나면 태스크 종료
	const bool isPlaying = mOwnerEnemy->GetMesh()->GetSingleNodeInstance()->IsPlaying();
	if (isPlaying == false)
	{
		const uint8 cached = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(CachedStateKey.SelectedKeyName);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsEnum(EnemyStateKey.SelectedKeyName, cached);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
