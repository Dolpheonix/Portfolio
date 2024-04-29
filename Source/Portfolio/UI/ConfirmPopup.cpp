#include "ConfirmPopup.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

UNumpadPopup::UNumpadPopup(const FObjectInitializer& objectInitializer) : Super(objectInitializer), bBound(false), mCurrentNumber(-1), mMaxNumber(-1)
{

}

void UNumpadPopup::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mIncreaseButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Increase")));
		mDecreaseButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Decrease")));
		mEditableNumBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Txt_Numpad")));

		mConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Confirm")));
		mExitButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Exit")));

		mTitleText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Title")));
	}
}

void UNumpadPopup::NativeConstruct()
{
	Super::NativeConstruct();

	check(mIncreaseButton);
	check(mDecreaseButton);
	check(mEditableNumBox);
	check(mConfirmButton);
	check(mExitButton);
	check(mTitleText);

	if (bBound == false)
	{
		mIncreaseButton->OnClicked.AddDynamic(this, &UNumpadPopup::OnClicked_Increase);
		mDecreaseButton->OnClicked.AddDynamic(this, &UNumpadPopup::OnClicked_Decrease);
		mConfirmButton->OnClicked.AddDynamic(this, &UNumpadPopup::OnClicked_Confirm);
		mEditableNumBox->OnTextCommitted.AddDynamic(this, &UNumpadPopup::OnCommitted_UpdateNumber);
		mExitButton->OnClicked.AddDynamic(this, &UNumpadPopup::OnClicked_Exit);

		bBound = true;
	}
}

void UNumpadPopup::SetTitle(FName str)
{
	mTitleText->SetText(FText::FromName(str));
}

void UNumpadPopup::SetNumpad(int max)
{
	mCurrentNumber = 1;
	mMaxNumber = max;

	UpdateNumpad();
}

void UNumpadPopup::UpdateNumpad()
{
	mEditableNumBox->SetText(FText::FromString(FString::FromInt(mCurrentNumber)));

	mIncreaseButton->SetIsEnabled(mCurrentNumber < mMaxNumber); // MAX 이상일 경우 increase button 비활성화
	mDecreaseButton->SetIsEnabled(mCurrentNumber > 1); // 1 이하일 경우에는 decrease button 비활성화
}

void UNumpadPopup::OnClicked_Increase()
{
	mCurrentNumber++;
	
	UpdateNumpad();
}

void UNumpadPopup::OnClicked_Decrease()
{
	mCurrentNumber--;

	UpdateNumpad();
}

void UNumpadPopup::OnCommitted_UpdateNumber(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (Text.IsNumeric() == true)
	{
		const int committedNumber = FCString::Atoi(*Text.ToString());
		
		if ((committedNumber >= 1) && (committedNumber <= mMaxNumber))
		{
			mCurrentNumber = committedNumber;
		}
	}

	UpdateNumpad();
}

void UNumpadPopup::OnClicked_Confirm()
{
	mNumpadConfirmEvent.Broadcast(mCurrentNumber);
}

void UNumpadPopup::OnClicked_Exit()
{
	RemoveFromParent();
}

UNotifyPopup::UNotifyPopup(const FObjectInitializer& objinit) : Super(objinit), bBound(false)
{
	
}

void UNotifyPopup::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mNotifyText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Notify")));
		mConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Confirm")));
	}
}

void UNotifyPopup::NativeConstruct()
{
	Super::NativeConstruct();

	check(mNotifyText);
	check(mConfirmButton);

	if (bBound == false)
	{
		mConfirmButton->OnClicked.AddDynamic(this, &UUserWidget::RemoveFromParent);

		bBound = true;
	}
}

void UNotifyPopup::SetNotifyText(FName str)
{
	mNotifyText->SetText(FText::FromName(str));
}
