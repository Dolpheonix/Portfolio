#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ShooterTask_Shot.generated.h"

class AShooter;

/*
	UShooterTask_Shot : Shooter ĳ������ ���� �½�ũ
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
	// ���� Ÿ��
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector TargetKey;
	// ���� ĳ����(EnemyCharacter)�� ������Ʈ ��
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;

private:
	// ���� ĳ����
	TObjectPtr<AShooter> mOwnerShooter;
};
