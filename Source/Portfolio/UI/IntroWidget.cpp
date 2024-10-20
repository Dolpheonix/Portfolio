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

		// FString에서 string으로 바꿔주는 작업
		const char* id = TCHAR_TO_ANSI(*mIdText->GetText().ToString());
		const char* pw = TCHAR_TO_ANSI(*mPwText->GetText().ToString());

		// 로그인이 실패했다면, 실패 팝업창을 띄움
		bool succeeded = gi->TryLogin(id, pw);
		if (succeeded == false)
		{
			NotifyLoginFailed(false);
		}
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
		
		bool succeeded = gi->TryRegister(id, pw);
		if (succeeded == false)
		{
			NotifyLoginFailed(true);
		}
		else
		{
			NotifyRegisterSucceeded();
		}
	}
}

void UIntroWidget::OnClicked_Submit()
{
	if (mNicknameText->GetText().IsEmpty() == false)
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);

		const char* nickname = TCHAR_TO_ANSI(*mNicknameText->GetText().ToString());

		bool succeeded = gi->SetNickname(nickname);
		if (succeeded == false)
		{
			NotifyNicknameDuplicated();
		}
	}
}

void UIntroWidget::NotifyLoginFailed(bool isRegister)
{
	// 팝업 UI는 한번만 생성하고, Visibility로 관리함
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
	mWidgetSwitcher->SetActiveWidgetIndex(1);	// 닉네임 설정 위젯으로 전환
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
