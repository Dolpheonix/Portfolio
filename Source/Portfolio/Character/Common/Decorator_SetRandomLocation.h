#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetRandomLocation.generated.h"

/*
	활동 반경 내의 랜덤 위치를 설정
	- 플레이어를 발견하기 전, 패트롤 할 때 쓰임
*/

UCLASS()
class PORTFOLIO_API UDecorator_SetRandomLocation : public UBTDecorator
{
	GENERATED_BODY()
	
	UDecorator_SetRandomLocation();

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& searchData) override;

protected:
	// 이동 중심점
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector Pivot;
	// 이동 반경
	UPROPERTY(EditAnywhere, Category="Custom")
	float Radius;
	// 설정된 랜덤 위치
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector Output;
};
