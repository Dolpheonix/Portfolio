#pragma once

#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "../Common/Interface/Interaction.h"
#include "../Common/Item.h"
#include "../Common/Dialogue.h"
#include "NpcCharacter.generated.h"

class ANpcController;
class APlayerCharacter;
class UCameraComponent;
class UBillboardComponent;

/*
	ANpcCharacter : �÷��̾�� ��ȣ�ۿ��� ������ ĳ����.
		- ��ȭ, ���� ���
*/

UCLASS()
class PORTFOLIO_API ANpcCharacter : public ACustomCharacter, public IInteraction
{
	GENERATED_BODY()
	
public:
	ANpcCharacter();

protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	
	// Interaction Override
	virtual void Notify(TObjectPtr<AActor> player) override;
	virtual void UnNotify(TObjectPtr<AActor> player) override;
	virtual void Interact(TObjectPtr<AActor> player) override;
	virtual void UnInteract(TObjectPtr<AActor> player) override;

	// Getter/Setter
	TObjectPtr<UCameraComponent> GetSelfCamera();
	TObjectPtr<ANpcController> GetNpcController();
	FInventory& GetShopItems();
	const FInventory& GetShopItems() const;
	TArray<FGameItem>& GetRewardItems();
	FDialogueList& GetDialogue();
	int GetIndex() const;
	bool IsInteracting() const;

	// ��ȭ, ���� ������ Json �����ͷ� �ε�
	bool LoadFromJson(const TSharedPtr<FJsonObject>& obj);

protected:
	// ���� ���� Npc�� �ε��� (data/leveldata/npc.json�� �ۼ��� npclist ���� �ε����� ��ġ�ؾ� ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int mNpcIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UCameraComponent> mSelfCamera;
	
	// �÷��̾��� ��ȣ�ۿ� ������ ������ ǥ���� �ȳ� �̹���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mNotifyBillboard;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ANpcController> mController;

	// NPC�� �������� �Ǹ��� ������ ���
	UPROPERTY(BlueprintReadWrite)
	FInventory mShopItems;

	UPROPERTY(BlueprintReadWrite)
	TArray<FGameItem> mRewardItems;

	// NPC�� ��ȣ�ۿ� �� ǥ���� ��ȭ ���
	UPROPERTY(BlueprintReadWrite)
	FDialogueList mDialogue;
	
	// ���� �÷��̾�� ��ȣ�ۿ� ���ΰ�?
	UPROPERTY(BlueprintReadWrite)
	bool bInteracting;
};
