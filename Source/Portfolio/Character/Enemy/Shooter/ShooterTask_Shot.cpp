#include "ShooterTask_Shot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "../../EnemyCharacter.h"
#include "../Shooter.h"

// ExecuteTask(), TickTask()
static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

UShooterTask_Shot::UShooterTask_Shot() : mOwnerShooter(nullptr)
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	// 각각 EEnemyState 열거형, AActor 클래스의 블래곱드 키만 선택 가능함.
	EnemyStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_Shot, EnemyStateKey), FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState")));
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_Shot, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UShooterTask_Shot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<UBlackboardComponent> blackboard = OwnerComp.GetAIOwner()->GetBlackboardComponent();

	mOwnerShooter = Cast<AShooter>(OwnerComp.GetAIOwner()->GetPawn());
	TObjectPtr<AActor> target = Cast<AActor>(blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	check(mOwnerShooter);
	check(target);

	// Detected 상태가 아닌데 공격을 할 수 없음.
	const EEnemyState state = static_cast<EEnemyState>(blackboard->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (state != EEnemyState::Detected) return EBTNodeResult::Failed;

	// 타겟을 향해 몸을 돌림
	const FVector dir = (target->GetActorLocation() - mOwnerShooter->GetActorLocation()).GetSafeNormal();
	const FRotator rot = FRotationMatrix::MakeFromX(dir).Rotator();
	mOwnerShooter->SetActorRotation(rot);

	// 해당 방향으로 발사
	mOwnerShooter->Shoot();

	// 발사 애니메이션이 끝날때까지 태스크를 진행해야 함
	return EBTNodeResult::InProgress;
}

void UShooterTask_Shot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 발사 애니메이션이 끝났거나, Detected 상태에서 벗어나게 되면 태스크를 종료
	const bool isPlaying = mOwnerShooter->GetMesh()->GetSingleNodeInstance()->IsPlaying();
	const EEnemyState state = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if ((isPlaying == false) || (state != EEnemyState::Detected))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
