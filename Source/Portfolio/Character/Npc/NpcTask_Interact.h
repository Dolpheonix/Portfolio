#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NpcTask_Interact.generated.h"

/*
	UNpcTask_Interact : ��ȣ �ۿ� ���¿� �����ϴ� �½�ũ. Ÿ�� �÷��̾ �ٶ󺸰�, ��ȣ�ۿ��� ���� ������ �½�ũ�� �����Ѵ�.
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
