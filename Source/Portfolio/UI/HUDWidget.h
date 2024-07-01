#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UProgressBar;
class UImage;
class APlayerCharacter;
class UScrollBox;
class UTextBlock;
class UEditableTextBox;
class UButton;
class UImage;

/*
	인게임에 표시되는 UI
	- HP 바
	- 퀵슬롯 이미지
	- 채팅창
*/
UCLASS()
class PORTFOLIO_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UHUDWidget(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	// 메인 UI를 엶
	void Open(TObjectPtr<APlayerCharacter> player);
	// 채팅창 활성화/비활성화
	void SetChattingBoxVisibility(bool newVisibility);
	bool GetChattingBoxVisibility() const;

	// 채팅창을 업데이트함
	void UpdateChatting();
	// HP바를 업데이트함
	void UpdateHpBar();
	// 퀵슬롯의 무기 이미지를 업데이트함
	void UpdateWeaponImage();
	// 채팅창에 채팅을 입력하면, 이를 서버로 전송하는 함수
	UFUNCTION()
	void SendChatting();

protected:
	// HP 바
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UProgressBar> mHpProgressBar;
	// 현재 선택된 무기 이미지
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mCurrentWeaponImage;
	// 퀵슬롯 키를 누를 시 선택되는 무기 이미지
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mNextWeaponImage;

	// 채팅창 위젯
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UScrollBox> mChattingScroll;
	// 채팅 텍스트 목록
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UTextBlock>> mChattingTexts;
	// 채팅 입력창
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UEditableTextBox> mChattingInputText;
	// 채팅 전송 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mSendChattingButton;
	// 채팅창 배경 (회색 필터)
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UImage> mChattingBoxBackground;

private:
	// 플레이어 캐릭터
	TObjectPtr<APlayerCharacter> mOwnerPlayer;
	// 채팅창 활성화/비활성화
	bool bChattingBoxVisibility;

	// 바인딩 중복 방지용 플래그
	bool bBound;
};
