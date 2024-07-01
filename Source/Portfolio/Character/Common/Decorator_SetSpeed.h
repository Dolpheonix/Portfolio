#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Decorator_SetSpeed.generated.h"

/*
	캐릭터의 이동 속도 설정
	- 평상 시와 플레이어를 발견했을 때의 움직임이 달라야 함
*/

UCLASS()
class PORTFOLIO_API UDecorator_SetSpeed : public UBTDecorator
{
	GENERATED_BODY()
	
	UDecorator_SetSpeed();

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;

protected:
	// 변경할 속도
	UPROPERTY(EditAnywhere, Category="Custom")
	float Speed;
};
