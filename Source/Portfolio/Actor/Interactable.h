#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Common/Interface/Interaction.h"
#include "Interactable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionEvent);

class UStaticMeshComponent;
class UBillboardComponent;
class UAIPerceptionStimuliSourceComponent;

/*
	AInteractable : �÷��̾�� ��ȣ�ۿ� ������ ����
*/

UCLASS()
class PORTFOLIO_API AInteractable : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:
	AInteractable();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Notify(TObjectPtr<AActor> player) override;
	virtual void UnNotify(TObjectPtr<AActor> player) override;
	virtual void Interact(TObjectPtr<AActor> player) override;

public:
	// ��ȣ �ۿ� �� ������ �̺�Ʈ (���� Ŭ������ ���� ����, ���� �������Ʈ���� ���ε� �� �� ����)
	UPROPERTY(BlueprintAssignable, Category="Event")
	FInteractionEvent mInteractionEvent;

protected:
	// ���� �޽�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> mStaticMeshComponent;
	// �÷��̾��� ���� ������ ������ �� ǥ���� �˸��� �̹���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBillboardComponent> mNotifyBillboard;
	// �÷��̾��� �ۼ��ǿ� �����Ǳ� ���� �ʿ��� ������Ʈ
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> mPerceptionStimuliSourceComponent;

};
