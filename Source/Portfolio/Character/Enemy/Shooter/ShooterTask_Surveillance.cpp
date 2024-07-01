#include "ShooterTask_Surveillance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../EnemyCharacter.h"

// ExecuteTask(), TickTask()
static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

UShooterTask_Surveillance::UShooterTask_Surveillance(const FObjectInitializer& objinit) : Super(objinit)
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
	
	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_Surveillance, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
}

EBTNodeResult::Type UShooterTask_Surveillance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Patrol 상태가 아니면 태스크 Fail
	EEnemyState Mode = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (Mode != EEnemyState::Patrol)
	{	
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UShooterTask_Surveillance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Patrol 상태에서 벗어났다면, 태스크 종료
	EEnemyState Mode = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (Mode != EEnemyState::Patrol)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
