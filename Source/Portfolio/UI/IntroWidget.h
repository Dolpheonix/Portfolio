#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmPopup.h"
#include "IntroWidget.generated.h"

class UButton;
class UTextButton;
class UTextBlock;
class UEditableTextBox;
class UWidgetSwitcher;

/*
	게임 시작전에 표시되는 인트로 UI
	- 로그인, 회원가입
	- 회원가입 성공 시 닉네임 설정
*/
UCLASS()
class PORTFOLIO_API UIntroWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UIntroWidget(const FObjectInitializer& objectInitializer);
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

protected:
	// 입력된 id, pw로 로그인 시도
	UFUNCTION()
	void OnClicked_Login();
	// 입력된 id, pw로 회원가입 시도
	UFUNCTION()
	void OnClicked_Register();
	// 입력된 닉네임 설정 시도
	UFUNCTION()
	void OnClicked_Submit();

public:
	// 로그인/회원가입이 실패했을 경우, 팝업창을 띄움
	void NotifyLoginFailed(bool isRegister);
	// 회원가입이 성공했을 경우, 닉네임 설정창을 띄움
	void NotifyRegisterSucceeded();
	// 닉네임 설정이 실패했을 경우, 팝업창을 띄움
	void NotifyNicknameDuplicated();

protected:
	// 로그인 화면 / 닉네임 설정 화면 전환용
	TObjectPtr<UWidgetSwitcher> mWidgetSwitcher;

// 로그인 화면
	// ID 입력 텍스트
	TObjectPtr<UEditableTextBox> mIdText;
	// PW 입력 텍스트
	TObjectPtr<UEditableTextBox> mPwText;
	// 로그인 버튼
	TObjectPtr<UButton> mLoginButton;
	// 회원가입 버튼
	TObjectPtr<UButton> mRegisterButton;

// 닉네임 설정 화면
	// 닉네임 입력 텍스트
	TObjectPtr<UEditableTextBox> mNicknameText;
	// 닉네임 제출 버튼
	TObjectPtr<UButton> mSubmitButton;

	// 알림 팝업창 클래스 
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UNotifyPopup> NotifyPopupClass;
	// 알림 팝업 UI
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNotifyPopup> mNotifyPopup;
private:
	// 중복 바인딩 방지용 플래그
	bool bBounded;
};
