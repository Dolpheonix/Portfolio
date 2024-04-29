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
	mSlotButtons.SetNum(MAX_SAVE_SLOT);
	NotifyPopupClass = LoadHelper::C_LoadObjectFromPath<UBlueprintGeneratedClass>(TEXT("/Game/UI/NotifyPopup_BP.NotifyPopup_BP_C"));
}

void UIntroWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mWidgetSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("Switcher")));
		mBackButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Back")));

		mNewGameButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_NewGame")));
		mLoadGameButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_LoadGame")));

		mNewNameText = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Txt_NewName")));
		mNewButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_New")));

		mSlotButtons[0] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_0")));
		mSlotButtons[1] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_1")));
		mSlotButtons[2] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_2")));
		mSlotButtons[3] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_3")));
		mSlotButtons[4] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_4")));
		mSlotButtons[5] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_5")));
		mSlotButtons[6] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_6")));
		mSlotButtons[7] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_7")));
		mSlotButtons[8] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_8")));
		mSlotButtons[9] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_9")));

		mSlotNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_SlotName")));
		mSlotInfoText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_SlotInfo")));
		mLoadButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Load")));
	}
}

void UIntroWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(mWidgetSwitcher);
	check(mBackButton);

	check(mNewGameButton);
	check(mLoadGameButton);
	check(mNewNameText);
	check(mNewButton);

	for (int i = 0; i < MAX_SAVE_SLOT; ++i)
	{
		check(mSlotButtons[i]);
	}

	check(mSlotNameText);
	check(mSlotInfoText);
	check(mLoadButton);

	if (bBounded == false)
	{
		mBackButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_Back);

		mNewGameButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_OpenNewGameMenu);
		mLoadGameButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_OpenLoadGameMenu);
		
		mNewButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_StartNewGame);
		
		for (int i = 0; i < MAX_SAVE_SLOT; ++i)
		{
			mSlotButtons[i]->mIndex = i;
			mSlotButtons[i]->mClickEvent.AddDynamic(this, &UIntroWidget::OnClicked_SelectSaveSlot);
		}
		
		mLoadButton->OnClicked.AddDynamic(this, &UIntroWidget::OnClicked_LoadGame);

		bBounded = true;
	}

	SwitchMenu(EIntroWidgetType::Main);
}

void UIntroWidget::OnClicked_Back()
{
	const EIntroWidgetType currType = static_cast<EIntroWidgetType>(mWidgetSwitcher->GetActiveWidgetIndex());
	switch (currType)
	{
	case EIntroWidgetType::Main:
		break;
	case EIntroWidgetType::NewGame:
	case EIntroWidgetType::LoadGame:
		SwitchMenu(EIntroWidgetType::Main);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Current Intro Widget Type is invalid"));
		break;
	}
}

void UIntroWidget::OnClicked_OpenNewGameMenu()
{
	SwitchMenu(EIntroWidgetType::NewGame);
}

void UIntroWidget::OnClicked_OpenLoadGameMenu()
{
	SwitchMenu(EIntroWidgetType::LoadGame);

	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	int slotNum = 0;

	for (int i = 0; i < MAX_SAVE_SLOT; ++i)
	{
		PlayerInfo info = gi->GetSaveData(i);
		
		if (info.CurrentGold < 0)
		{
			mSlotButtons[i]->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			mSlotButtons[i]->SetVisibility(ESlateVisibility::Visible);
			mSlotButtons[i]->mTextBlock->SetText(FText::FromString(info.PlayerName));
		}
	}


	mLoadButton->SetIsEnabled(false);
}

void UIntroWidget::OnClicked_StartNewGame()
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const FText newName = mNewNameText->GetText();
	if (newName.IsEmpty() == true)
	{
		// 이름이 입력되지 않음
		ShowPopup(TEXT("이름을 입력하세요"));

		return;
	}

	const int savedSlot = gi->CreateSaveFile(mNewNameText->GetText().ToString());
	if (savedSlot < 0)
	{
		ShowPopup(TEXT("저장 슬롯이 가득 찼습니다"));
		return;
	}

	gi->LoadGame(savedSlot);
}

void UIntroWidget::OnClicked_SelectSaveSlot(int index)
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	PlayerInfo selectedInfo = gi->GetSaveData(index);

	mSelectedSlot = index;

	mSlotNameText->SetText(FText::FromString(selectedInfo.PlayerName));
	
	FString infoText;
	infoText = "Level : " + FString::FromInt(selectedInfo.CurrentLevel);
	infoText += "\n";
	infoText += "Map : " + gi->GetLevelName(selectedInfo.CurrentMap);

	mSlotInfoText->SetText(FText::FromString(infoText));

	mLoadButton->SetIsEnabled(true);
}

void UIntroWidget::OnClicked_LoadGame()
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	gi->LoadGame(mSelectedSlot);
}

void UIntroWidget::SwitchMenu(EIntroWidgetType type)
{
	mWidgetSwitcher->SetActiveWidgetIndex(static_cast<uint8>(type));
	
	mBackButton->SetVisibility((type == EIntroWidgetType::Main) ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

void UIntroWidget::ShowPopup(FName notifyText)
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

	mNotifyPopup->SetNotifyText(notifyText);
}
