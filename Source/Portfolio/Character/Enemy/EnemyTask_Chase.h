#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "EnemyTask_Chase.generated.h"

/*
	UEnemyTask_Chase : Ÿ�� �÷��̾ �Ѵ� �½�ũ. Ÿ���� ������� �½�ũ ����
						(MoveTo �½�ũ�� ��ӹ޾�, ���� ������ �Ÿ������� �ѵ��� ������ �� ����.)
*/

UCLASS()
class PORTFOLIO_API UEnemyTask_Chase : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UEnemyTask_Chase(const FObjectInitializer& objinit);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// ���� ĳ������ State ��
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
};
