#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyTask_Hurt.generated.h"

class AEnemyCharacter;

/*
	UEnemyTask_Hurt : 캐릭터가 데미지를 입었을 때 실행되는 태스크.
		- 애니메이션이 실행되는 동안 경직 상태를 유지
		- 태스크가 끝나면 원래 State 값으로 변경한다.
*/

UCLASS()
class PORTFOLIO_API UEnemyTask_Hurt : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UEnemyTask_Hurt();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 현재 State 값 (EEnemyState::Hurt여야 함)
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector EnemyStateKey;
	// 데미지를 입기 전의 State 값
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector CachedStateKey;

private:
	// 오너 캐릭터
	TObjectPtr<AEnemyCharacter> mOwnerEnemy;
};
