#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetRandomLocation.generated.h"

/*
	UDecorator_SetRandomLocation : 기준 위치로부터 일정 거리만큼 떨어진 랜덤 위치를 설정하는 데코레이터
*/

UCLASS()
class PORTFOLIO_API UDecorator_SetRandomLocation : public UBTDecorator
{
	GENERATED_BODY()
	
	UDecorator_SetRandomLocation();

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& searchData) override;

protected:
	// 기준 위치
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector Pivot;
	// 기준 위치로부터의 거리
	UPROPERTY(EditAnywhere, Category="Custom")
	float Radius;
	// 설정된 랜덤 위치를 담을 블랙보드 키
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector Output;
};
