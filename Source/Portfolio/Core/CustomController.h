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
	// 각 UI가 뷰포트에 추가/해제될 때 호출해 부가적인 처리
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

	// UI 외부에서 발생한 이벤트를 UI에 적용하기 위한 메서드
	void UpdateChatting();						// 서버로부터 전달된 채팅을 UI에 적용
	void UpdateHealthBar();						// HUD의 체력 바 업데이트
	void UpdateWeaponImage();					// 퀵슬롯 변경 시 이미지 업데이트
	void NotifyLoginFailed(bool isRegister);	// 로그인/회원가입 실패 시 알립창 팝업
	void NotifyRegisterSucceeded();				// 회원가입 성공 시 닉네임 설정창으로 변경
	void NotifyNicknameDuplicated();			// 닉네임 중복 시 알림창 팝업

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
