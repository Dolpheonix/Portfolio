#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Common/Interface/Interaction.h"
#include "../Common/Item.h"
#include "RootItem.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;
class UBillboardComponent;
class UAIPerceptionStimuliSourceComponent;

/*
	ARootItem : �÷��̾ ��ȣ�ۿ� �� �������� ȹ���� �� �ִ� ����
*/

UCLASS()
class PORTFOLIO_API ARootItem : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	ARootItem();

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Notify(TObjectPtr<AActor> player) override;
	virtual void UnNotify(TObjectPtr<AActor> player) override;
	virtual void Interact(TObjectPtr<AActor> player) override;

protected:
	// ��ƼŬ ������ ���δ� ��Ͽ� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCapsuleComponent> mCapsuleComponent;
	// ��ƼŬ ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraComponent> mNiagaraComponent;
	// �÷��̾��� ��ȣ�ۿ� ���� ������ ������ �� ǥ���� �̹���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBillboardComponent> mNotifyBillboard;
	// �÷��̾��� �ۼ��ǿ� �����Ǳ� ���� �ʿ��� ������Ʈ
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> mPerceptionStimuliSourceComponent;
	
	// ��ȣ �ۿ�� �÷��̾ ȹ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	FGameItem Item;
};
