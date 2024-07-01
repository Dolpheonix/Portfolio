#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "NpcTask_Stroll.generated.h"

/*
	정해진 반경 내에서 이동하는 태스크
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
	// 현재 상호작용 중인가?
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector IsInteracting_Key;
};
