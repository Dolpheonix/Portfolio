#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NpcTask_Interact.generated.h"

/*
	상호작용 태스크. 타겟을 응시하는 것 외에는 Idle 태스크와 동일
*/

UCLASS()
class PORTFOLIO_API UNpcTask_Interact : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UNpcTask_Interact();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 현재 상호작용 중인가?
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector IsInteracting_Key;
	// 상호작용 타겟
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector InteractTarget_Key;
};
