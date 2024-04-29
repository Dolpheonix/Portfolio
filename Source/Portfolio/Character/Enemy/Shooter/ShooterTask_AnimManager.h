#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ShooterTask_AnimManager.generated.h"

/*
	UShooterTask_AnimManager : Shooter 캐릭터의 애니메이션 트랜지션을 관리하는 태스크
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
	// Move 애니메이션으로 전환할 지를 결정
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Move_Key;
	// Idle 애니메이션으로 전환할 지를 결정
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Idle_Key;
	// Attack 애니메이션으로 전환할 지를 결정
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Attack_Key;
	// Return 애니메이션으로 전환할 지를 결정
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Return_Key;
	// Hurt 애니메이션으로 전환할 지를 결정
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Hurt_Key;
};
