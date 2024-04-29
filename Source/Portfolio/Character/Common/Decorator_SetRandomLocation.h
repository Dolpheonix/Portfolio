#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetRandomLocation.generated.h"

/*
	UDecorator_SetRandomLocation : ���� ��ġ�κ��� ���� �Ÿ���ŭ ������ ���� ��ġ�� �����ϴ� ���ڷ�����
*/

UCLASS()
class PORTFOLIO_API UDecorator_SetRandomLocation : public UBTDecorator
{
	GENERATED_BODY()
	
	UDecorator_SetRandomLocation();

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& searchData) override;

protected:
	// ���� ��ġ
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector Pivot;
	// ���� ��ġ�κ����� �Ÿ�
	UPROPERTY(EditAnywhere, Category="Custom")
	float Radius;
	// ������ ���� ��ġ�� ���� ������ Ű
	UPROPERTY(EditAnywhere, Category = "Custom")
	FBlackboardKeySelector Output;
};
