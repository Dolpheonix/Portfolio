#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "EnemyTask_Chase.generated.h"

/*
	EnemyCharacter의 추격 태스크
	- 타겟을 공격 가능 범위에 들어올때까지 쫓아감
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
	// Enemy State
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
};
