#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "ShooterTask_Surveillance.generated.h"

/*
	UShooterTask_Surveillance : ������ ��ġ�� �̵��ϴ� �½�ũ. �÷��̾ �����ϸ� �½�ũ�� �����.
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
	// ���� ĳ������ State Ű
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FBlackboardKeySelector EnemyStateKey;
};
