#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyTask_Hurt.generated.h"

class AEnemyCharacter;

/*
	데미지를 입으면 일정 시간동안 경직에 걸리는 태스크
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
	// Enemy State
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
	// 태스크 종료 후 되돌려놓을 이전 State
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector CachedStateKey;

private:
	// 태스크 Onwer
	TObjectPtr<AEnemyCharacter> mOwnerEnemy;
};
