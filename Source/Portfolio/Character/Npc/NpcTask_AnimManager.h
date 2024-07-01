#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NpcTask_AnimManager.generated.h"

/*
	Npc 애니메이션 관리용 태스크. Parallel Task로 실행
*/

UCLASS()
class PORTFOLIO_API UNpcTask_AnimManager : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UNpcTask_AnimManager();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// Move 애니메이션 재생용 Boolean
	UPROPERTY(EditAnywhere, Category="BlackboardKey")
	FBlackboardKeySelector Transition_Move_Key;
	// Idle 애니메이션 재생용 Boolean
	UPROPERTY(EditAnywhere, Category="BlackboardKey")
	FBlackboardKeySelector Transition_Idle_Key;
	// Interact 애니메이션 재생용 Boolean
	UPROPERTY(EditAnywhere, Category="BlackboardKey")
	FBlackboardKeySelector Transition_Interact_Key;
};
