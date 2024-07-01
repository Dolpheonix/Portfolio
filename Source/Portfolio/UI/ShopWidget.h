#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Common/Item.h"
#include "ShopWidget.generated.h"

/* 
	상점 UI
	- NPC 아이템 목록
	- 플레이어 아이템 목록
	- 아이템 구매/판매 기능
*/

class ANpcCharacter;
class APlayerCharacter;
class UIndexedButton;
class UButton;
class UNumpadPopup;
class UTextBlock;

UCLASS()
class PORTFOLIO_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

	UShopWidget(const FObjectInitializer& objectInitializer);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void Open(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc);
	void Close();

protected:
	// 아이템 탭을 변경함
	// @param isNpcTab : NPC 아이템 쪽의 버튼인가?
	void ChangeTab(int index, bool isNpcTab);
	// 탭의 강조 상태를 업데이트함
	void UpdateTab(bool isNpcTab);
	// 탭에 맞게 아이템 슬롯창을 업데이트함
	void UpdateItemSlot(bool isNpcTab);
	// 골드 보유량을 업데이트함
	void UpdateCurrentGold();
	// 구매/판매 시 개수를 결정하는 팝업창을 띄움
	void OpenConfirmPopup(int index, bool isBuying);

	// 플레이어 쪽의 탭 버튼을 클릭할 때 호출되는 함수. 탭 버튼 업데이트 + 슬롯창 업데이트
	UFUNCTION()
	void OnClicked_ChangeTab_Ch(int index);
	// NPC 쪽의 탭 버튼을 클릭할 때 호출되는 함수. 탭 버튼 업데이트 + 슬롯창 업데이트
	UFUNCTION()
	void OnClicked_ChangeTab_Npc(int index);

	// NPC 쪽 아이템 슬롯을 클릭했을 때 호출되는 함수. 구매 확인창을 띄움
	UFUNCTION()
	void OnClicked_ConfirmBuy(int index);
	// 플레이어 쪽 아이템 슬롯을 클릭했을 때 호출되는 함수. 판매 확인창을 띄움
	UFUNCTION()
	void OnClicked_ConfirmSell(int index);
	// NPC 쪽 아이템 슬롯을 호버했을 때 호출되는 함수. 개당 구매 가격을 표시
	UFUNCTION()
	void OnHovered_ShowBuyPrice(int index);
	// 플레이어 쪽 아이템 슬롯을 호버했을 때 호출되는 함수. 개당 판매 가격을 표시
	UFUNCTION()
	void OnHovered_ShowSellPrice(int index);
	// 커서가 슬롯창을 벗어날 때 호출되는 함수. 가격 표시를 지움
	UFUNCTION()
	void OnUnhovered_RemovePrice();

	// 개수 결정 팝업창에서 확인 버튼을 클릭하면 호출되는 함수. 구매/판매를 진행
	// @param num : 구매/판매할 개수
	UFUNCTION()
	void OnClicked_BuySell(int num);

	// 종료버튼을 누름. UI 종료
	UFUNCTION()
	void OnClicked_Exit();

protected:
	// 플레이어쪽 탭 버튼
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mTabButtons_Character;
	// NPC쪽 탭 버튼
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mTabButtons_Npc;

	// 플레이어쪽 아이템 슬롯창
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mItemSlotButtons_Character;
	// NPC쪽 아이템 슬롯창
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mItemSlotButtons_Npc;

	// 플레이어쪽 아이템 개수 텍스트
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mItemNumbers_Character;
	// NPC쪽 아이템 개수 텍스트
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mItemNumbers_Npc;

	// 종료 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;
	
	// 가격 텍스트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mPriceText;
	// 골드 보유량 텍스트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mCurrentGoldText;

	// 개수 결정 팝업창 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	TSubclassOf<UNumpadPopup> mNumpadPopupClass;
	// 팝업창 UI
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNumpadPopup> mConfirmPopup;

private:
	// NPC
	TObjectPtr<ANpcCharacter> mOwnerNpc;
	// 플레이어 캐릭터
	TObjectPtr<APlayerCharacter> mOwnerPlayer;

	// 현재 선택된 플레이어쪽 탭
	EItemType mCurrentTab_Ch;
	// 현재 선택된 NPC쪽 탭
	EItemType mCurrentTab_Npc;

	// 빈 슬롯용 브러시
	FSlateBrush mSlotBrush_Empty;
	// 선택된 탭 전용 브러시
	FSlateBrush mTabBrush_Bright;
	// 선택되지 않은 탭 브러시
	FSlateBrush mTabBrush_Dark;

	// 현재 구매/판매를 진행하는 아이템의 인벤토리상 인덱스
	int mSelectedIndex;
	// 현재 구매중인가? 판매중인가?
	bool bBuying;

	// 중복 바인딩 방지용 플래그
	bool bBound;

};
