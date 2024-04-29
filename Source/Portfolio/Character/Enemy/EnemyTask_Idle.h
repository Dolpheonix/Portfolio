#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "EnemyTask_Idle.generated.h"

/*
	UEnemyTask_Idle : Patrol 중 특정 위치에 도달하면 일정 시간동안 대기하는 태스크
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
	// 오너 캐릭터의 스테이트 키
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
};
