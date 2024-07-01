#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericTeamAgentInterface.h"
#include "Interaction.generated.h"

/*
	플레이어와 상호작용 가능한 액터들의 인터페이스
	- 상호작용/해제
	- 범위에 들어왔을 때 알림/해제
*/

UINTERFACE(MinimalAPI)
class UInteraction : public UGenericTeamAgentInterface
{
	GENERATED_BODY()
};

class PORTFOLIO_API IInteraction : public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(1); };

	// 상호작용 범위에 들어왔음을 알림
	virtual void Notify(TObjectPtr<AActor> player) {};	// Why? : 언리얼 클래스에서는 순수 가상함수를 선언할 수 없음
	// 상호작용 범위에서 벗어났음을 알림
	virtual void UnNotify(TObjectPtr<AActor> player) {};
	// 플레이어와 상호작용
	virtual void Interact(TObjectPtr<AActor> player) {};
	// 상호작용 해제
	virtual void UnInteract(TObjectPtr<AActor> player) {};
};
