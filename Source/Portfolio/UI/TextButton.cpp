#include "TextButton.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UTextButton::UTextButton(const FObjectInitializer& objectInitializer) : Super(objectInitializer), mIndex(-1), bBounded(false), FontSize(32.0f)
{
}

void UTextButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	mTextBlock	= Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
	mButton		= Cast<UButton>(GetWidgetFromName(TEXT("Button")));

	mNormalColor = FSlateColor(FLinearColor::Black);
	mHoveredColor = FSlateColor(FLinearColor::Yellow);
	mPressedColor = FSlateColor(FLinearColor::Red);

	if (mTextBlock)
	{
		mTextBlock->SetText(Text);

		FSlateFontInfo tempFont = mTextBlock->GetFont();
		tempFont.Size = FontSize;
		mTextBlock->SetFont(tempFont);
	}
}

void UTextButton::NativeConstruct()
{
	Super::NativeConstruct();

	check(mTextBlock);
	check(mButton);
	
	if (bBounded == false)
	{
		mButton->OnHovered.AddDynamic(this, &UTextButton::OnHovered_ChangeTextColor);
		mButton->OnUnhovered.AddDynamic(this, &UTextButton::OnUnHovered_ChangeTextColor);
		mButton->OnPressed.AddDynamic(this, &UTextButton::OnPressed_ChangeTextColor);
		mButton->OnReleased.AddDynamic(this, &UTextButton::OnReleased_ChangeTextColor);
		mButton->OnClicked.AddDynamic(this, &UTextButton::ClickTrigger);

		bBounded = true;
	}
}

void UTextButton::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	const FString& name = e.Property->GetName();
	if (name == "FontSize")
	{
		if (!mTextBlock)
		{
			mTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
		}
		if(mTextBlock)
		{
			FSlateFontInfo tempFont = mTextBlock->GetFont();
			tempFont.Size = FontSize;
			mTextBlock->SetFont(tempFont);
		}
	}
	else if (name == "Text")
	{
		if (!mTextBlock)
		{
			mTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
		}
		if (mTextBlock)
		{
			mTextBlock->SetText(Text);
		}
	}
}

void UTextButton::ClickTrigger()
{
	mClickEvent.Broadcast(mIndex);
}

void UTextButton::OnHovered_ChangeTextColor()
{
	mTextBlock->SetColorAndOpacity(mHoveredColor);
}

void UTextButton::OnUnHovered_ChangeTextColor()
{
	mTextBlock->SetColorAndOpacity(mNormalColor);
}

void UTextButton::OnPressed_ChangeTextColor()
{
	mTextBlock->SetColorAndOpacity(mPressedColor);
}

void UTextButton::OnReleased_ChangeTextColor()
{
	mTextBlock->SetColorAndOpacity(mNormalColor);
}