#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetBoolean.generated.h"

/*
	UDecorator_SetBoolean : ��忡 ������ �� �οﰪ�� �����Ű�� ���ڷ�����
*/

UCLASS()
class PORTFOLIO_API UDecorator_SetBoolean : public UBTDecorator
{
	GENERATED_BODY()
	
	UDecorator_SetBoolean();

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& searchData) override;

protected:
	UPROPERTY(EditAnywhere, Category="Custom")
	FBlackboardKeySelector BlackboardKey;

	UPROPERTY(EditAnywhere, Category="Custom")
	bool BooleanValue;
};
