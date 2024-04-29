#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "EnemyTask_Idle.generated.h"

/*
	UEnemyTask_Idle : Patrol �� Ư�� ��ġ�� �����ϸ� ���� �ð����� ����ϴ� �½�ũ
*/

UCLASS()
class PORTFOLIO_API UEnemyTask_Idle : public UBTTask_Wait
{
	GENERATED_BODY()
public:
	UEnemyTask_Idle(const FObjectInitializer& objinit);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// ���� ĳ������ ������Ʈ Ű
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
};
