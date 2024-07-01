#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmPopup.generated.h"

/*
	팝업창 모음
*/

class UTextBlock;
class UButton;
class UEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNumpadClickConfirmEvent, int, num);

/*
	TextBox로 숫자를 입력할 수 있고, 증가/감소 버튼이 존재해 개수를 결정할 수 있는 팝업창
	- EX : 상점에서 구매할 아이템의 개수
*/
UCLASS()
class PORTFOLIO_API UNumpadPopup : public UUserWidget
{
	GENERATED_BODY()

	UNumpadPopup(const FObjectInitializer& objectInitializer);
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	// 팝업창 제목 설정
	void SetTitle(FName str);
	// 숫자 입력창 초기화
	// @param max : 입력창에 최대 입력 가능한 수
	void SetNumpad(int max);
	// 숫자가 바뀌었을 때 UI를 업데이트하는 함수
	void UpdateNumpad();

	// 수를 1만큼 증가시키는 함수
	UFUNCTION()
	void OnClicked_Increase();
	// 수를 1만큼 감소시키는 함수
	UFUNCTION()
	void OnClicked_Decrease();
	// 입력된 텍스트를 통해 숫자를 업데이트하는 함수
	UFUNCTION()
	void OnCommitted_UpdateNumber(const FText& Text, ETextCommit::Type CommitMethod);
	// 확인 버튼을 눌렀을 때, 정해진 수를 인자로 보내 이벤트를 실행하는 함수
	UFUNCTION()
	void OnClicked_Confirm();
	// X 버튼을 눌렀을 때, UI 창을 닫는 함수
	UFUNCTION()
	void OnClicked_Exit();

public:
	// 숫자 증가 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mIncreaseButton;
	// 숫자 감소 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mDecreaseButton;
	// 숫자 입력 텍스트창
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UEditableTextBox> mEditableNumBox;
	// 확인 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mConfirmButton;
	// X(종료) 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mExitButton;
	// 팝업창 타이틀
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mTitleText;
	// 확인 버튼을 눌렀을 때 실행될 이벤트
	FNumpadClickConfirmEvent mNumpadConfirmEvent; // confirm ��ư�� ������ �� mCurrentNumber�� ���ڷ� ����Ǵ� �̺�Ʈ

private:
	// 위젯, 이벤트 바인딩이 중복되지 않게 하는 플래그
	bool bBound;
	// 현재 설정된 숫자
	int mCurrentNumber;
	// 최대로 설정할 수 있는 숫자
	int mMaxNumber;
};

/*
	단순히 메시지를 노출하는 알림용 팝업창 
*/
UCLASS()
class PORTFOLIO_API UNotifyPopup : public UUserWidget
{
	GENERATED_BODY()

public:
	UNotifyPopup(const FObjectInitializer& objinit);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	// 알림 메시지를 설정하는 함수
	void SetNotifyText(FName str);

protected:
	// 알림 메시지
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextBlock> mNotifyText;
	// 확인 버튼
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton> mConfirmButton;

private:
	bool bBound;
};

UCLASS()
class PORTFOLIO_API UConfirmPopup : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//virtual void NativePreConstruct() override;

};
