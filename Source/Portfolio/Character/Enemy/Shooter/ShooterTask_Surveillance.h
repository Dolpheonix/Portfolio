#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "ShooterTask_Surveillance.generated.h"

/*
	UShooterTask_Surveillance : 지정된 위치로 이동하는 태스크. 플레이어를 감지하면 태스크를 벗어난다.
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
	// 오너 캐릭터의 State 키
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FBlackboardKeySelector EnemyStateKey;
};
