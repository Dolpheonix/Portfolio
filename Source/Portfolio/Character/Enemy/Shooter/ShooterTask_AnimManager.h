#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ShooterTask_AnimManager.generated.h"

/*
	UShooterTask_AnimManager : Shooter ĳ������ �ִϸ��̼� Ʈ�������� �����ϴ� �½�ũ
*/

UCLASS()
class PORTFOLIO_API UShooterTask_AnimManager : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UShooterTask_AnimManager();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// Move �ִϸ��̼����� ��ȯ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Move_Key;
	// Idle �ִϸ��̼����� ��ȯ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Idle_Key;
	// Attack �ִϸ��̼����� ��ȯ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Attack_Key;
	// Return �ִϸ��̼����� ��ȯ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Return_Key;
	// Hurt �ִϸ��̼����� ��ȯ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Hurt_Key;
};
