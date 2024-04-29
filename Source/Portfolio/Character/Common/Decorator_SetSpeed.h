#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetSpeed.generated.h"

/*
	UDecorator_SetSpeed : ĳ������ �ӵ��� �����ϴ� ���ڷ�����
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
