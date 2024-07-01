#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ShooterTask_Shot.generated.h"

class AShooter;

/*
	Shooter 몹의 공격 태스크
*/

UCLASS()
class PORTFOLIO_API UShooterTask_Shot : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UShooterTask_Shot();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 공격 타겟
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector TargetKey;
	// 캐릭터 State
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;

private:
	// Owner 캐릭터
	TObjectPtr<AShooter> mOwnerShooter;
};
