#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetBoolean.generated.h"

/*
	블랙보드 Boolean 변수를 변경
	- 애니메이션 설정용 불리안 변수 (Idle, Walk, Run, Attack)
*/

UCLASS()
class PORTFOLIO_API UDecorator_SetBoolean : public UBTDecorator
{
	GENERATED_BODY()
	
	UDecorator_SetBoolean();

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& searchData) override;

protected:
	// 변경할 불리안 변수
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector BlackboardKey;
	// 설정값
	UPROPERTY(EditAnywhere, Category="Custom")
	bool BooleanValue;
};
