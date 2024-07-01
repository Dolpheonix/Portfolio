#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NpcTask_Idle.generated.h"

/*
	Npc용 Idle 태스크
*/

UCLASS()
class PORTFOLIO_API UNpcTask_Idle : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UNpcTask_Idle();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 태스크 지속 시간
	UPROPERTY(EditAnywhere, Category="Custom")
	float IdleTime;
	// Interacting 상태인가?
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector IsInteracting_Key;
};
