#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Common/Item.h"
#include "ConfirmPopup.h"
#include "InventoryWidget.generated.h"

class APlayerCharacter;
class APreviewCharacter;
class UIndexedButton;
class UTextBlock;
class UImage;
class UButton;

/*
	인벤토리 UI
	- 아이템 종류별 탭 버튼
	- 종류별 보유 아이템 표시
	- 프리뷰 캐릭터
	- 아이템 버리기
	- 보유 골드량 표시
*/

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
	// UI를 엶
	void Open(TObjectPtr<APlayerCharacter> player);
	// UI를 닫음
	void Close();

protected:
	// 탭 버튼 업데이트 (선택된 탭 강조)
	void UpdateTab();
	// 선택된 탭의 아이템으로 슬롯창 업데이트
	void UpdateItemSlot();
	// 아이템을 선택
	// @param index : 인벤토리상 인덱스
	void SelectItem(int index);
	// 아이템의 선택을 취소
	// @param index : 인벤토리상 인덱스
	void UnselectItem(int index);
	// 아이템을 버리기 전, 개수를 정하는 팝업창을 띄움
	void ShowDeletePopup();

	// 탭버튼을 클릭할 때 호출되는 함수. 눌린 탭으로 전환, 슬롯창 업데이트
	// @param index : 탭버튼 인덱스
	UFUNCTION()
	void OnClicked_ChangeTab(int index);
	// 아이템 슬롯을 클릭할 때 호술되는 함수. 아이템을 선택 
	UFUNCTION()
	void OnClicked_SelectItem(int index);
	// 아이템 슬롯을 호버할 때 호출되는 함수. 아이템의 설명을 표시
	UFUNCTION()
	void OnHovered_ShowExplanation(int index);
	// 아이템 슬롯을 눌렀을 때 호출되는 함수. 드래그 효과 적용
	UFUNCTION()
	void OnPressed_DragSlot(int index);
	// 아이템 슬롯을 커서가 벗어날 때 호출. 설명창에서 설명을 지움
	UFUNCTION()
	void OnUnhovered_RemoveExplanation();
	// 버리기 버튼에 드래그 이미지를 놓을 때 호출되는 함수. 드래그된 아이템을 버림
	UFUNCTION()
	void OnReleased_ThrowItem();
	// X 버튼을 클릭할 때 호출되는 함수. UI를 닫음
	UFUNCTION()
	void OnClicked_Exit();
	// 팝업창의 확인 버튼을 클릭할 때 호출되는 함수. 정해진 개수만큼 아이템을 버림
	UFUNCTION()
	void OnClicked_DeleteItem(int num);

protected:
	// 탭 버튼
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mTabButtons;
	// 아이템 슬롯
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UIndexedButton>> mItemSlotButtons;
	// 아이템 개수를 표시하는 텍스트
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mItemNumbers;

	// 버리기 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mThrowButton;
	// 종료 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;

	// 프리뷰 캐릭터를 표시하는 이미지
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mPreviewImage;
	// 설명 텍스트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mExplanationText;
	// 골드 보유량 텍스트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mCurrentGoldText;
	// 드래그된 아이템 이미지 (반투명하게 설정)
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mDraggedImage;

	// 아이템을 버릴 때 개수를 결정하는 팝업창 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TSubclassOf<UNumpadPopup> mNumpadPopupClass;
	// 팝업창 UI
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNumpadPopup> mThrowPopup;

	// 플레이어 캐릭터
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	// 프리뷰 캐릭터
	TObjectPtr<APreviewCharacter> mPreviewClone;

	// 현재 선택된 탭
	EItemType mCurrentTab;

	// 빈 슬롯 표시용 브러시
	FSlateBrush mSlotBrush_Empty;
	// 선택된 탭 전용 브러시
	FSlateBrush mTabBrush_Bright;
	// 선택되지 않은 탭 전용 브러시
	FSlateBrush mTabBrush_Dark;

private:
	// 중복 바인딩 방지용 플래그
	bool bBound;
	// 현재 이미지를 드래그 중인가?
	bool bDragging;
	// 드래그된 아이템의 인덱스
	int mDraggedIndex;
};
