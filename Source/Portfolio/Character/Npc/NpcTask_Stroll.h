#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "NpcTask_Stroll.generated.h"

/*
	UNpcTask_Stroll : 특정 위치로 이동하는 태스크. 위치 도착/상호작용 시에 태스크 종료
*/

UCLASS()
class PORTFOLIO_API UNpcTask_Stroll : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UNpcTask_Stroll(const FObjectInitializer& objinit);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector IsInteracting_Key;
};
