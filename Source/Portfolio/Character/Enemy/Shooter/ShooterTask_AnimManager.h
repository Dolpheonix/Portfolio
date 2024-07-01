#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ShooterTask_AnimManager.generated.h"

/*
	Shooter 몹의 애니메이션을 관리하는 태스크
	- Parallel Task로 실행됨
	- Idle, Move, Attack, Hurt, Return
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
	// Move Animation 설정용 Boolean
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Move_Key;
	// Idle Animation 설정용 Boolean
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Idle_Key;
	// Attack Animation 설정용 Boolean
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Attack_Key;
	// Return Animation 설정용 Boolean
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Return_Key;
	// Hurt Animation 설정용 Boolean
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
	FBlackboardKeySelector Transition_Hurt_Key;
};
