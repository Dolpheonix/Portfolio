#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "ShooterTask_Surveillance.generated.h"

/*
	Shooter 몹의 패트롤 태스크
	- 플레이어 감지 시 스테이트 전환
*/

UCLASS()
class PORTFOLIO_API UShooterTask_Surveillance : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UShooterTask_Surveillance(const FObjectInitializer& objinit);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 몹 State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FBlackboardKeySelector EnemyStateKey;
};
