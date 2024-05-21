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

UCLASS()
class PORTFOLIO_API UIntroWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UIntroWidget(const FObjectInitializer& objectInitializer);
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnClicked_Login();
	UFUNCTION()
	void OnClicked_Register();
	UFUNCTION()
	void OnClicked_Submit();

public:
	// �α��� ���и� �˸��� �˾�â�� ���
	void NotifyLoginFailed(bool isRegister);
	// ȸ������ ������ �˸�. �г��� ���� â���� ��ȯ
	void NotifyRegisterSucceeded();
	// �г����� �ߺ����� �˸��� �˾�â�� ���
	void NotifyNicknameDuplicated();

protected:
	TObjectPtr<UWidgetSwitcher> mWidgetSwitcher;

	// Login Session
	TObjectPtr<UEditableTextBox> mIdText;
	TObjectPtr<UEditableTextBox> mPwText;
	TObjectPtr<UButton> mLoginButton;
	TObjectPtr<UButton> mRegisterButton;

	// Set Nickname Session
	TObjectPtr<UEditableTextBox> mNicknameText;
	TObjectPtr<UButton> mSubmitButton;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UNotifyPopup> NotifyPopupClass;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNotifyPopup> mNotifyPopup;

	int mSelectedSlot;

private:
	bool bBounded;
};
