#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyTask_StareAt.generated.h"

/*
	UEnemyTask_StareAt : Enemy ĳ���Ͱ� Caution ���¿� �����ϸ� ����Ǵ� �½�ũ. Ÿ���� �Ĵٺ�.
*/

UCLASS()
class PORTFOLIO_API UEnemyTask_StareAt : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UEnemyTask_StareAt();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
};
