#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericTeamAgentInterface.h"
#include "Interaction.generated.h"

/*
	UInteraction : 상호작용 가능한 오브젝트들이 사용할 함수를 모아놓은 인터페이스
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

	// 상호 작용 가능한 상태에 진입했을 때 호출
	virtual void Notify(TObjectPtr<AActor> player) {};
	// 상호 작용 가능한 상태에서 벗어났을 때 호출
	virtual void UnNotify(TObjectPtr<AActor> player) {};
	// 상호 작용 시작
	virtual void Interact(TObjectPtr<AActor> player) {};
	// 상호 작용 끝남
	virtual void UnInteract(TObjectPtr<AActor> player) {};
};
