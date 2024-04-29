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
	ANpcCharacter : 플레이어와 상호작용이 가능한 캐릭터.
		- 대화, 상점 기능
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

	// 대화, 상점 정보를 Json 데이터로 로드
	bool LoadFromJson(const TSharedPtr<FJsonObject>& obj);

protected:
	// 레벨 상의 Npc의 인덱스 (data/leveldata/npc.json에 작성된 npclist 상의 인덱스와 일치해야 함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int mNpcIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UCameraComponent> mSelfCamera;
	
	// 플레이어의 상호작용 범위에 들어오면 표시할 안내 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mNotifyBillboard;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ANpcController> mController;

	// NPC의 상점에서 판매할 아이템 목록
	UPROPERTY(BlueprintReadWrite)
	FInventory mShopItems;

	UPROPERTY(BlueprintReadWrite)
	TArray<FGameItem> mRewardItems;

	// NPC와 상호작용 시 표시할 대화 목록
	UPROPERTY(BlueprintReadWrite)
	FDialogueList mDialogue;
	
	// 현재 플레이어와 상호작용 중인가?
	UPROPERTY(BlueprintReadWrite)
	bool bInteracting;
};
