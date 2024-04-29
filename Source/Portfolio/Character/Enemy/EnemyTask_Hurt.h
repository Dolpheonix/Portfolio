#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyTask_Hurt.generated.h"

class AEnemyCharacter;

/*
	UEnemyTask_Hurt : ĳ���Ͱ� �������� �Ծ��� �� ����Ǵ� �½�ũ.
		- �ִϸ��̼��� ����Ǵ� ���� ���� ���¸� ����
		- �½�ũ�� ������ ���� State ������ �����Ѵ�.
*/

UCLASS()
class PORTFOLIO_API UEnemyTask_Hurt : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UEnemyTask_Hurt();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// ���� State �� (EEnemyState::Hurt���� ��)
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
	// �������� �Ա� ���� State ��
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector CachedStateKey;

private:
	// ���� ĳ����
	TObjectPtr<AEnemyCharacter> mOwnerEnemy;
};
