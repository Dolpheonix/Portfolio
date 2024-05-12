#include "HUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/CustomGameInstance.h"
#include "../Core/CustomController.h"
#include "../Character/PlayerCharacter.h"
#include "../Base/LoadHelper.h"

UHUDWidget::UHUDWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer), bChattingBoxVisibility(false), bBound(false)
{
	mChattingTexts.SetNum(MAX_CHATTING_TO_SHOW);
}

void UHUDWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	bIsFocusable = true;

	if (IsDesignTime() == false)
	{
		mHpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("Pgb_Hp")));
		mCurrentWeaponImage = Cast<UImage>(GetWidgetFromName(TEXT("Img_CurrWeapon")));
		mNextWeaponImage = Cast<UImage>(GetWidgetFromName(TEXT("Img_NextWeapon")));

		mChattingScroll = Cast<UScrollBox>(GetWidgetFromName(TEXT("Scb_Chatting")));

		mChattingTexts[0] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_0")));
		mChattingTexts[1] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_1")));
		mChattingTexts[2] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_2")));
		mChattingTexts[3] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_3")));
		mChattingTexts[4] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_4")));
		mChattingTexts[5] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_5")));
		mChattingTexts[6] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_6")));
		mChattingTexts[7] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_7")));
		mChattingTexts[8] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_8")));
		mChattingTexts[9] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_9")));
		mChattingTexts[10] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_10")));
		mChattingTexts[11] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_11")));
		mChattingTexts[12] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_12")));
		mChattingTexts[13] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_13")));
		mChattingTexts[14] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_14")));
		mChattingTexts[15] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_15")));
		mChattingTexts[16] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_16")));
		mChattingTexts[17] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_17")));
		mChattingTexts[18] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_18")));
		mChattingTexts[19] = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Chat_19")));

		mChattingInputText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Txt_ChattingInput")));
		mSendChattingButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_SendChatting")));
		mChattingBoxBackground = Cast<UImage>(GetWidgetFromName(TEXT("Img_ChatBackground")));
	}
}

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(mHpProgressBar);
	check(mCurrentWeaponImage);
	check(mNextWeaponImage);

	check(mChattingScroll);
	for (int i = 0; i < MAX_CHATTING_TO_SHOW; ++i)
	{
		check(mChattingTexts[i]);
	}
	check(mChattingInputText);
	check(mSendChattingButton);
	check(mChattingBoxBackground);

	if (bBound == false)
	{
		mSendChattingButton->OnClicked.AddDynamic(this, &UHUDWidget::SendChatting);

		bBound = true;
	}
}

FReply UHUDWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if((InKeyEvent.GetCharacter() == 72) || (InKeyEvent.GetCharacter() == 104))
	{
		TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
		check(pc);
		pc->CloseChattingBox();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UHUDWidget::Open(TObjectPtr<APlayerCharacter> player)
{
	check(player);

	mOwnerPlayer = player;

	UpdateHpBar();
	UpdateWeaponImage();

	SetChattingBoxVisibility(false);
}

void UHUDWidget::SetChattingBoxVisibility(bool newVisibility)
{
	mChattingScroll->SetVisibility(newVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	mChattingInputText->SetVisibility(newVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	mSendChattingButton->SetVisibility(newVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	mChattingBoxBackground->SetVisibility(newVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	bChattingBoxVisibility = newVisibility;
	if (newVisibility == true)
	{
		mChattingScroll->ScrollToEnd();
	}
}

bool UHUDWidget::GetChattingBoxVisibility() const
{
	return bChattingBoxVisibility;
}

void UHUDWidget::UpdateChatting()
{
	UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);
	const TArray<FString>& chats = gi->GetSavedChattings();

	for (int i = 0; i < MAX_CHATTING_TO_SHOW; ++i)
	{
		mChattingTexts[i]->SetText(FText::FromString(chats[i]));
	}
}

void UHUDWidget::UpdateHpBar()
{
	float percent = static_cast<float>(mOwnerPlayer->GetMaxHp()) / mOwnerPlayer->GetCurrHp();
	mHpProgressBar->SetPercent(percent);
}

void UHUDWidget::UpdateWeaponImage()
{	
	const int currIndex = mOwnerPlayer->GetCurrentWeapon();
	if (currIndex < 0)
	{
		// 보유중인 무기가 없다
		mCurrentWeaponImage->Brush.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Thumbnail/Empty_Normal.Empty_Normal")));
		mNextWeaponImage->Brush.SetResourceObject(LoadHelper::LoadObjectFromPath<UTexture2D>(TEXT("/Game/Texture/WidgetImage/Thumbnail/Empty_Normal.Empty_Normal")));

		return;
	}
	
	const int nextIndex = (currIndex + 1) % mOwnerPlayer->GetInventory().GetTypeInventory(EItemType::Weapon).ItemList.Num();

	UCustomGameInstance* gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const ItemInfo& currInfo = gi->GetItemInfo(mOwnerPlayer->GetInventory().GetTypeInventory(EItemType::Weapon).ItemList[currIndex].InfoIndex);
	const ItemInfo& nextInfo = (currIndex == nextIndex) ? currInfo : gi->GetItemInfo(mOwnerPlayer->GetInventory().GetTypeInventory(EItemType::Weapon).ItemList[nextIndex].InfoIndex);

	mCurrentWeaponImage->Brush.SetResourceObject(currInfo.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
	mNextWeaponImage->Brush.SetResourceObject(nextInfo.Thumbnail[static_cast<uint8>(EThumbnailType::Normal)]);
}

void UHUDWidget::SendChatting()
{
	if (mChattingInputText->GetText().IsEmpty() == false)
	{
		TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
		check(gi);

		const char* msg = TCHAR_TO_ANSI(*mChattingInputText->GetText().ToString());
		gi->SendChatMessage(msg);

		mChattingInputText->SetText(FText::GetEmpty());
	}
}
