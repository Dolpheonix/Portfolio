#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Common/Item.h"
#include "ShopWidget.generated.h"

/* 상점 위젯
	1. 판매 목록 슬롯
	2. 구매 목록 슬롯
	3. 아이템 설명
	4. 보유 금액
	5. 나가기 버튼
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
	// Tab을 바꾼 후, 탭에 맞게 슬롯을 업데이트
	void ChangeTab(int index, bool isNpcTab);
	// 탭 버튼 업데이트
	void UpdateTab(bool isNpcTab);
	// 슬롯 버튼 업데이트
	void UpdateItemSlot(bool isNpcTab);
	// 보유 골드량 없데이트
	void UpdateCurrentGold();
	// 구매/판매 확인 팝업 열기
	void OpenConfirmPopup(int index, bool isBuying);

	// TabButton
	UFUNCTION()
	void OnClicked_ChangeTab_Ch(int index);
	UFUNCTION()
	void OnClicked_ChangeTab_Npc(int index);

	// ItemButton
	UFUNCTION()
	void OnClicked_ConfirmBuy(int index);
	UFUNCTION()
	void OnClicked_ConfirmSell(int index);
	UFUNCTION()
	void OnHovered_ShowBuyPrice(int index);
	UFUNCTION()
	void OnHovered_ShowSellPrice(int index);
	UFUNCTION()
	void OnUnhovered_RemovePrice();

	// ConfirmPopup
	UFUNCTION()
	void OnClicked_BuySell(int num);

	// ExitButton
	UFUNCTION()
	void OnClicked_Exit();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mTabButtons_Character;
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mTabButtons_Npc;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mItemSlotButtons_Character;
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mItemSlotButtons_Npc;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mItemNumbers_Character;
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mItemNumbers_Npc;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mPriceText;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mCurrentGoldText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	TSubclassOf<UNumpadPopup> mNumpadPopupClass;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNumpadPopup> mConfirmPopup;

private:
	TObjectPtr<ANpcCharacter> mOwnerNpc;
	TObjectPtr<APlayerCharacter> mOwnerPlayer;

	EItemType mCurrentTab_Ch;
	EItemType mCurrentTab_Npc;

	FSlateBrush mSlotBrush_Empty;
	FSlateBrush mTabBrush_Bright;
	FSlateBrush mTabBrush_Dark;

	int mSelectedIndex;
	bool bBuying;

	bool bBound;

};
