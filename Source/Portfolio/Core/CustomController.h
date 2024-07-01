#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../UI/IntroWidget.h"
#include "../UI/HUDWidget.h"
#include "../UI/MenuWidget.h"
#include "../UI/InventoryWidget.h"
#include "../UI/QuestWidget.h"
#include "../UI/DialogueWidget.h"
#include "../UI/ShopWidget.h"
#include "../UI/NotifyWidget.h"
#include "CustomController.generated.h"

class APlayerCharacter;
class ANpcCharacter;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

/*
	 플레이어 컨트롤러 클래스
	 - UI 관리
	 - 시각 퍼셉션 : 주변 상호작용 오브젝트를 감지하기 위함
*/
UCLASS()
class PORTFOLIO_API ACustomController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACustomController();

protected:
	virtual void BeginPlay() override;

public:
	// 인트로 UI를 엶
	void OpenIntro();
	// 인트로 UI를 닫음
	void CloseIntro();
	// 메뉴 UI를 엶
	void OpenMenu(TObjectPtr<APlayerCharacter> player);
	// 메뉴 UI를 닫음
	void CloseMenu(TObjectPtr<APlayerCharacter> player, bool openIntro);
	// 안개암 UI를 엶
	void OpenHUD(TObjectPtr<APlayerCharacter> player);
	// 다이얼로그 UI를 엶
	void OpenDialogue(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc);
	// 다이얼로그 UI를 닫음
	void CloseDialogue(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc, bool openShop);
	// 인벤토리 UI를 엶
	void OpenInventory(TObjectPtr<APlayerCharacter> player);
	// 인벤토리 UI를 닫음
	void CloseInventory(TObjectPtr<APlayerCharacter> player);
	// 상점 UI를 엶
	void OpenShop(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc);
	// 상점 UI를 닫음
	void CloseShop(TObjectPtr<APlayerCharacter> player);
	// 퀘스트 UI를 엶
	void OpenQuestTable(TObjectPtr<APlayerCharacter> player);
	// 퀘스트 UI를 닫음
	void CloseQuestTable(TObjectPtr<APlayerCharacter> player);
	// 채팅창 UI를 엶
	void OpenChattingBox();
	// 채팅창 UI를 닫음
	void CloseChattingBox();

	// UI 코드 외부의 변화로 인해 UI를 업데이트해야 하는 경우, 이 클래스를 거쳐야 함
	// 채팅창을 업데이트함
	void UpdateChatting();
	// HP 바를 업데이트함
	void UpdateHealthBar();	
	// 퀵슬롯의 무기 이미지를 업데이트함
	void UpdateWeaponImage();

	TObjectPtr<UAIPerceptionComponent> GetPerceptionComponent();

public:
	// 인트로 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UIntroWidget> IntroWidgetClass;
	// 인게임 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UHUDWidget> HUDWidgetClass;
	// 메뉴 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget")
	TSubclassOf<UMenuWidget> MenuWidgetClass;
	// 인벤토리 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	// 퀘스트 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UQuestWidget> QuestWidgetClass;
	// 다이얼로그 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;
	// 상점 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UShopWidget> ShopWidgetClass;
	// 알림 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UNotifyWidget> NotifyWidgetClass;

private:
	// 인트로 UI
	TObjectPtr<UIntroWidget> mIntroWidget;
	// 인게임 UI
	TObjectPtr<UHUDWidget> mHUDWidget;
	// 메뉴 UI
	TObjectPtr<UMenuWidget> mMenuWidget;
	// 인벤토리 UI
	TObjectPtr<UInventoryWidget> mInventoryWidget;
	// 퀘스트 UI
	TObjectPtr<UQuestWidget> mQuestWidget;
	// 다이얼로그 UI
	TObjectPtr<UDialogueWidget> mDialogueWidget;
	// 상점 UI
	TObjectPtr<UShopWidget> mShopWidget;
	// 알림 UI
	TObjectPtr<UNotifyWidget> mNotifyWidget;

	// 시각 퍼셉션 컴포넌트
	TObjectPtr<UAIPerceptionComponent> mPerceptionComponent;
	// 시각 퍼셉션 설정
	TObjectPtr<UAISenseConfig_Sight> mSenseConfig_Sight;
};
