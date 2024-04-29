#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "NpcTask_Stroll.generated.h"

/*
	UNpcTask_Stroll : Ư�� ��ġ�� �̵��ϴ� �½�ũ. ��ġ ����/��ȣ�ۿ� �ÿ� �½�ũ ����
*/

UCLASS()
class PORTFOLIO_API UNpcTask_Stroll : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UNpcTask_Stroll(const FObjectInitializer& objinit);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector IsInteracting_Key;
};
