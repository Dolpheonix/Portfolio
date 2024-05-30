#include "IntroWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/CustomController.h"
#include "../Core/CustomGameInstance.h"
#include "../Base/LoadHelper.h"
#include "TextButton.h"

UIntroWidget::UIntroWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
																		, mSelectedSlot(-1)
																		, bBounded(false)
{
	NotifyPopupClass = LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/NotifyPopup_BP.NotifyPopup_BP_C"));
}

void UIntroWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mWidgetSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("Stw_switcher")));

		mIdText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Txt_Id")));
		mPwText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Txt_Pw")));
		mLoginButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Login")));
		mRegisterButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Register")));
	
		mNicknameText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Txt_Nickname")));
		mSubmitButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Submit")));
	}
}

void UIntroWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(mIdText);
	check(mPwText);
	check(mLoginButton);
	check(mRegisterButton);

	if (bBounded == false)
	{
		mLoginButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_Login);
		mRegisterButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_Register);
		mSubmitButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_Submit);

		bBounded = true;
	}
}

void UIntroWidget::OnClicked_Login()
{
	if ((mIdText->GetText().IsEmpty() == false) && (mPwText->GetText().IsEmpty() == false))
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);

		const char* id = TCHAR_TO_ANSI(*mIdText->GetText().ToString());
		const char* pw = TCHAR_TO_ANSI(*mPwText->GetText().ToString());
		gi->SendLoginInfo(id, pw, false);
	}
}

void UIntroWidget::OnClicked_Register()
{
	if ((mIdText->GetText().IsEmpty() == false) && (mPwText->GetText().IsEmpty() == false))
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);

		const char* id = TCHAR_TO_ANSI(*mIdText->GetText().ToString());
		const char* pw = TCHAR_TO_ANSI(*mPwText->GetText().ToString());
		gi->SendLoginInfo(id, pw, true);
	}
}

void UIntroWidget::OnClicked_Submit()
{
	if (mNicknameText->GetText().IsEmpty() == false)
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);

		const char* nickname = TCHAR_TO_ANSI(*mNicknameText->GetText().ToString());

		gi->SendNickname(nickname);
	}
}

void UIntroWidget::NotifyLoginFailed(bool isRegister)
{
	if (!mNotifyPopup)
	{
		mNotifyPopup = WidgetTree->ConstructWidget<UNotifyPopup>(NotifyPopupClass, TEXT("Popup"));

		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mNotifyPopup);
	}
	else
	{
		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mNotifyPopup);
	}

	Cast<UCanvasPanelSlot>(mNotifyPopup->Slot)->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));

	FName notifyStr = isRegister ? TEXT("Register Failed. Choose Another ID") : TEXT("Login Failed. Check your ID and PW");
	mNotifyPopup->SetNotifyText(notifyStr);
}

void UIntroWidget::NotifyRegisterSucceeded()
{
	mWidgetSwitcher->SetActiveWidgetIndex(1);
}

void UIntroWidget::NotifyNicknameDuplicated()
{
	if (!mNotifyPopup)
	{
		mNotifyPopup = WidgetTree->ConstructWidget<UNotifyPopup>(NotifyPopupClass, TEXT("Popup"));

		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mNotifyPopup);
	}
	else
	{
		TObjectPtr<UCanvasPanel> rootCanvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		rootCanvas->AddChild(mNotifyPopup);
	}

	Cast<UCanvasPanelSlot>(mNotifyPopup->Slot)->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));

	FName notifyStr = TEXT("Nickname Duplicated. Choose other nickname.");
	mNotifyPopup->SetNotifyText(notifyStr);
}
