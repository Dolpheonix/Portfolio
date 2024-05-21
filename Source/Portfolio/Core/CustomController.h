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
	 ACustomController : 게임 기본 플레이어 컨트롤러.
		- UI 클래스 설정
		- UI 열기/닫기 기능
		- 오브젝트 감지용 퍼셉션
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
	void OpenIntro();
	void CloseIntro();
	void OpenMenu(TObjectPtr<APlayerCharacter> player);
	void CloseMenu(TObjectPtr<APlayerCharacter> player, bool openIntro);
	void OpenHUD(TObjectPtr<APlayerCharacter> player);
	void OpenDialogue(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc);
	void CloseDialogue(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc, bool openShop);
	void OpenInventory(TObjectPtr<APlayerCharacter> player);
	void CloseInventory(TObjectPtr<APlayerCharacter> player);
	void OpenShop(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc);
	void CloseShop(TObjectPtr<APlayerCharacter> player);
	void OpenQuestTable(TObjectPtr<APlayerCharacter> player);
	void CloseQuestTable(TObjectPtr<APlayerCharacter> player);
	void OpenChattingBox();
	void CloseChattingBox();

	void UpdateChatting();
	void UpdateHealthBar();
	void UpdateWeaponImage();
	void NotifyLoginFailed(bool isRegister);
	void NotifyRegisterSucceeded();
	void NotifyNicknameDuplicated();

	// Getter
	TObjectPtr<UAIPerceptionComponent> GetPerceptionComponent();

public:
	// 인트로 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UIntroWidget> IntroWidgetClass;
	// 인게임 기본 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UHUDWidget> HUDWidgetClass;
	// 인게임 메뉴 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget")
	TSubclassOf<UMenuWidget> MenuWidgetClass;
	// 인벤토리 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	// 퀘스트 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UQuestWidget> QuestWidgetClass;
	// 대화 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;
	// 상점 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UShopWidget> ShopWidgetClass;
	// TODO : 알림팝업 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UNotifyWidget> NotifyWidgetClass;

private:
	TObjectPtr<UIntroWidget> mIntroWidget;
	TObjectPtr<UHUDWidget> mHUDWidget;
	TObjectPtr<UMenuWidget> mMenuWidget;
	TObjectPtr<UInventoryWidget> mInventoryWidget;
	TObjectPtr<UQuestWidget> mQuestWidget;
	TObjectPtr<UDialogueWidget> mDialogueWidget;
	TObjectPtr<UShopWidget> mShopWidget;
	TObjectPtr<UNotifyWidget> mNotifyWidget;

	TObjectPtr<UAIPerceptionComponent> mPerceptionComponent;
	TObjectPtr<UAISenseConfig_Sight> mSenseConfig_Sight;
};
