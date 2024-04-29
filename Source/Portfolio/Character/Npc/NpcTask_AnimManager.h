#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NpcTask_AnimManager.generated.h"

/*
	UNpcTask_AnimManager : NPC ĳ������ �ִϸ��̼� ������ �½�ũ
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
	// Move �ִϸ��̼����� ��ȯ�ϱ� ���� ������ Ű
	UPROPERTY(EditAnywhere, Category="BlackboardKey")
	FBlackboardKeySelector Transition_Move_Key;
	// Idle �ִϸ��̼����� ��ȯ�ϱ� ���� ������ Ű
	UPROPERTY(EditAnywhere, Category="BlackboardKey")
	FBlackboardKeySelector Transition_Idle_Key;
	// Interact �ִϸ��̼����� ��ȯ�ϱ� ���� ������ Ű
	UPROPERTY(EditAnywhere, Category="BlackboardKey")
	FBlackboardKeySelector Transition_Interact_Key;
};
