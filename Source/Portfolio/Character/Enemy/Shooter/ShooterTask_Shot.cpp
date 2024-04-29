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

	// ���� EEnemyState ������, AActor Ŭ������ ������ Ű�� ���� ������.
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

	// Detected ���°� �ƴѵ� ������ �� �� ����.
	const EEnemyState state = static_cast<EEnemyState>(blackboard->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if (state != EEnemyState::Detected) return EBTNodeResult::Failed;

	// Ÿ���� ���� ���� ����
	const FVector dir = (target->GetActorLocation() - mOwnerShooter->GetActorLocation()).GetSafeNormal();
	const FRotator rot = FRotationMatrix::MakeFromX(dir).Rotator();
	mOwnerShooter->SetActorRotation(rot);

	// �ش� �������� �߻�
	mOwnerShooter->Shoot();

	// �߻� �ִϸ��̼��� ���������� �½�ũ�� �����ؾ� ��
	return EBTNodeResult::InProgress;
}

void UShooterTask_Shot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// �߻� �ִϸ��̼��� �����ų�, Detected ���¿��� ����� �Ǹ� �½�ũ�� ����
	const bool isPlaying = mOwnerShooter->GetMesh()->GetSingleNodeInstance()->IsPlaying();
	const EEnemyState state = static_cast<EEnemyState>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsEnum(EnemyStateKey.SelectedKeyName));
	if ((isPlaying == false) || (state != EEnemyState::Detected))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
