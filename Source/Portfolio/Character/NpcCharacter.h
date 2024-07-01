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
	플레이어와 대화 가능한 Npc 캐릭터
	- 상호작용 : 대화
	- 상점 기능
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
	
	// 상호작용 범위에 들어왔음을 알림
	virtual void Notify(TObjectPtr<AActor> player) override;
	// 상호작용 범위를 벗어났음을 알릴
	virtual void UnNotify(TObjectPtr<AActor> player) override;
	// 상호작용 시작
	virtual void Interact(TObjectPtr<AActor> player) override;
	// 상호작용 끝
	virtual void UnInteract(TObjectPtr<AActor> player) override;

	TObjectPtr<UCameraComponent> GetSelfCamera();
	TObjectPtr<ANpcController> GetNpcController();
	FInventory& GetShopItems();
	const FInventory& GetShopItems() const;
	TArray<FGameItem>& GetRewardItems();
	FDialogueList& GetDialogue();
	int GetIndex() const;
	bool IsInteracting() const;

	// Json Object로부터 Npc 정보(다이얼로그, 상점)를 받아옴
	bool LoadFromJson(const TSharedPtr<FJsonObject>& obj);

protected:
	// 맵 상의 Npc 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int mNpcIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UCameraComponent> mSelfCamera;
	// 알림용 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBillboardComponent> mNotifyBillboard;
	// Npc 컨트롤러
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ANpcController> mController;
	// 상점 아이템 목록
	UPROPERTY(BlueprintReadWrite)
	FInventory mShopItems;
	// 보상 아이템 목록 (대화중에 이벤트로 획득)
	UPROPERTY(BlueprintReadWrite)
	TArray<FGameItem> mRewardItems;

	// 대화 목록
	UPROPERTY(BlueprintReadWrite)
	FDialogueList mDialogue;
	
	// 상호작용 중인가?
	UPROPERTY(BlueprintReadWrite)
	bool bInteracting;
};
