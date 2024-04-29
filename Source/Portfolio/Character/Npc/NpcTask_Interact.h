#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NpcTask_Interact.generated.h"

/*
	UNpcTask_Interact : 상호 작용 상태에 진입하는 태스크. 타겟 플레이어를 바라보고, 상호작용이 끝날 때까지 태스크를 유지한다.
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
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector IsInteracting_Key;

	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector InteractTarget_Key;
};
