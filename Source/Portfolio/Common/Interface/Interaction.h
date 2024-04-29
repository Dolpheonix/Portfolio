#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericTeamAgentInterface.h"
#include "Interaction.generated.h"

/*
	UInteraction : ��ȣ�ۿ� ������ ������Ʈ���� ����� �Լ��� ��Ƴ��� �������̽�
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

	// ��ȣ �ۿ� ������ ���¿� �������� �� ȣ��
	virtual void Notify(TObjectPtr<AActor> player) {};
	// ��ȣ �ۿ� ������ ���¿��� ����� �� ȣ��
	virtual void UnNotify(TObjectPtr<AActor> player) {};
	// ��ȣ �ۿ� ����
	virtual void Interact(TObjectPtr<AActor> player) {};
	// ��ȣ �ۿ� ����
	virtual void UnInteract(TObjectPtr<AActor> player) {};
};
