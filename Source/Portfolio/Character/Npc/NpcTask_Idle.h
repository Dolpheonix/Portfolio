#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NpcTask_Idle.generated.h"

/*
	UNpcTask_Idle : Ư�� �ð����� ������ �ִ� �½�ũ. ��ȣ �ۿ� �ÿ� �½�ũ Ż��
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
	UPROPERTY(EditAnywhere, Category="Custom")
	float IdleTime;
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector IsInteracting_Key;
};
