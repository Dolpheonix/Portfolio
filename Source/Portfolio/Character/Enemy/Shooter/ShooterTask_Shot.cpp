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

	// Detected 상태가 아니면 태스크 Fail 처리
	const EEnemyState state = static_cast<EEnemyState>(blackboard->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (state != EEnemyState::Detected) return EBTNodeResult::Failed;

	// 타겟을 바라보게끔 회전
	const FVector dir = (target->GetActorLocation() - mOwnerShooter->GetActorLocation()).GetSafeNormal();
	const FRotator rot = FRotationMatrix::MakeFromX(dir).Rotator();
	mOwnerShooter->SetActorRotation(rot);

	// 정면으로 발사
	mOwnerShooter->Shoot();

	return EBTNodeResult::InProgress;
}

void UShooterTask_Shot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 애니메이션이 끝날 때까지 태스크 지속
	const bool isPlaying = mOwnerShooter->GetMesh()->GetSingleNodeInstance()->IsPlaying();
	const EEnemyState state = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if ((isPlaying == false) || (state != EEnemyState::Detected))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
