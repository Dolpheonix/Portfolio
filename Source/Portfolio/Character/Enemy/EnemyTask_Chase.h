#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "EnemyTask_Chase.generated.h"

/*
	UEnemyTask_Chase : 타겟 플레이어를 쫓는 태스크. 타겟이 사라지면 태스크 종료
						(MoveTo 태스크를 상속받아, 공격 가능한 거리까지만 쫓도록 설정할 수 있음.)
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
	// 오너 캐릭터의 State 값
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
};
