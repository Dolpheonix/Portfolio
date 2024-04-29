#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Common/Item.h"
#include "ConfirmPopup.h"
#include "InventoryWidget.generated.h"

/* 인벤토리 위젯
	1. 아이템 목록 슬롯
		1) 의상, 무기, 기타 탭
		2) 슬롯에 선택 기능은 의상, 무기 탭에서만 가능하게
	2. 버리기 버튼 (드래그해서 버튼에 놓으면 확인 창 후 버려짐)
	3. 프리뷰 씬
	4. 아이템 설명 (Hovered 시에 나타나게)
	5. 보유 금액
	6. 나가기 버튼
*/	

class APlayerCharacter;
class APreviewCharacter;
class UIndexedButton;
class UTextBlock;
class UImage;
class UButton;

UCLASS()
class PORTFOLIO_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UInventoryWidget(const FObjectInitializer& objInitializer);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& Mygeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	void Open(TObjectPtr<APlayerCharacter> player);
	void Close();

protected:
	void UpdateTab();
	void UpdateItemSlot();
	void SelectItem(int index);
	void UnselectItem(int index);
	void ShowDeletePopup();

	// for Tab button
	UFUNCTION()
	void OnClicked_ChangeTab(int index);
	// for Item Slot
	UFUNCTION()
	void OnClicked_SelectItem(int index);
	UFUNCTION()
	void OnHovered_ShowExplanation(int index);
	UFUNCTION()
	void OnPressed_DragSlot(int index);
	UFUNCTION()
	void OnUnhovered_RemoveExplanation();
	UFUNCTION()
	void OnReleased_ThrowItem();
	// for exit button
	UFUNCTION()
	void OnClicked_Exit();
	// for popup ui
	UFUNCTION()
	void OnClicked_DeleteItem(int num);

protected:
	// widgets
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mTabButtons;
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mItemSlotButtons;
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mItemNumbers;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mThrowButton;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mPreviewImage;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mExplanationText;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mCurrentGoldText;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mDraggedImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<UNumpadPopup> mNumpadPopupClass;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNumpadPopup> mThrowPopup;

	// variables
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	TObjectPtr<APreviewCharacter> mPreviewClone;

	EItemType mCurrentTab;
	FSlateBrush mSlotBrush_Empty;
	FSlateBrush mTabBrush_Bright;
	FSlateBrush mTabBrush_Dark;

private:
	bool bBound;
	bool bDragging;
	int mDraggedIndex;
};
