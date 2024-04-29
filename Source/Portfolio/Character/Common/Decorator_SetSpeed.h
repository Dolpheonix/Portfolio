#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetSpeed.generated.h"

/*
	UDecorator_SetSpeed : 캐릭터의 속도를 변경하는 데코레이터
*/

UCLASS()
class PORTFOLIO_API UDecorator_SetSpeed : public UBTDecorator
{
	GENERATED_BODY()
	
	UDecorator_SetSpeed();

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;

protected:
	UPROPERTY(EditAnywhere, Category="Custom")
	float Speed;
};
