#include "QuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/PlayerCharacter.h"
#include "../Core/CustomGameInstance.h"
#include "../Core/CustomController.h"
#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "../Common/Enemy.h"
#include "TextButton.h"

UQuestWidget::UQuestWidget(const FObjectInitializer& objinit) : Super(objinit), mSelectedIndex(-1), bBound(false)
{
	mQuestSlotButtons.SetNum(MAX_QUEST_COMMIT);
}

void UQuestWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		mQuestSlotButtons[0] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_0")));
		mQuestSlotButtons[1] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_1")));
		mQuestSlotButtons[2] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_2")));
		mQuestSlotButtons[3] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_3")));
		mQuestSlotButtons[4] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_4")));
		mQuestSlotButtons[5] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_5")));
		mQuestSlotButtons[6] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_6")));
		mQuestSlotButtons[7] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_7")));
		mQuestSlotButtons[8] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_8")));
		mQuestSlotButtons[9] = Cast<UTextButton>(GetWidgetFromName(TEXT("Btn_Slot_9")));

		mExitButton = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Exit")));

		mQuestNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_QuestName")));
		mQuestExplanationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_QuestExplanation")));
		mSubQuestExplanationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_SubExplanation")));
	}
}

void UQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (int i = 0; i < MAX_QUEST_COMMIT; ++i)
	{
		check(mQuestSlotButtons[i]);
	}

	check(mExitButton);

	check(mQuestNameText);
	check(mQuestExplanationText);
	check(mSubQuestExplanationText);

	if (bBound == false)
	{
		for (int i = 0; i < MAX_QUEST_COMMIT; ++i)
		{
			mQuestSlotButtons[i]->mClickEvent.AddDynamic(this, &UQuestWidget::OnClicked_SelectQuest);
		}

		mExitButton->OnClicked.AddDynamic(this, &UQuestWidget::OnClicked_Exit);

		bBound = true;
	}
}

void UQuestWidget::Open(TObjectPtr<APlayerCharacter> player)
{
	check(player);
	mOwnerPlayer = player;

	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	const TArray<FQuestStatus>& qt = mOwnerPlayer->GetQuestTable();
	
	int slotCnt = 0;
	for (int i = 0; i < qt.Num(); ++i)
	{
		if ((qt[i].CurrProgress == EQuestProgressType::InProgess) || (qt[i].CurrProgress == EQuestProgressType::Completable))
		{
			const FQuest& q = gi->GetQuestInfo(i);
			mQuestSlotButtons[slotCnt]->mTextBlock->SetText(FText::FromString(q.Name));
			mQuestSlotButtons[slotCnt]->mIndex = i;
			mQuestSlotButtons[slotCnt]->SetVisibility(ESlateVisibility::Visible);
			slotCnt++;
		}
	}

	for (int i = slotCnt; i < MAX_QUEST_COMMIT; ++i)
	{
		mQuestSlotButtons[i]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UQuestWidget::Close()
{
	mOwnerPlayer = nullptr;
}

void UQuestWidget::OnClicked_SelectQuest(int index)
{
	TObjectPtr<UCustomGameInstance> gi = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(gi);

	check(index < gi->GetQuestInfoList().Num());
	const FQuest& selectedQuest = gi->GetQuestInfo(index);
	const FQuestStatus& selectedStatus = mOwnerPlayer->GetQuestTable()[index];

	mQuestNameText->SetText(FText::FromString(selectedQuest.Name));
	mQuestExplanationText->SetText(FText::FromString(selectedQuest.Explanation));

	FString explanationStr;
	for (int i = 0; i < selectedQuest.SubQuests.Num(); ++i)
	{
		FString str = FString::FromInt(i + 1);
		str += ". ";

		switch (selectedQuest.SubQuests[i].Type)
		{
		case ESubQuestType::Arrival:
		case ESubQuestType::Action:
			str += FString::Printf(TEXT("%s"), *selectedQuest.SubQuests[i].Explanation);
			break;
		case ESubQuestType::Hunt:
		{
			const FEnemyLabelInfo& info = gi->GetEnemyLabelInfo(selectedQuest.SubQuests[i].HuntInfo.Index);
			str += FString::Printf(TEXT("%s ~ %d/%d"), *selectedQuest.SubQuests[i].Explanation, selectedStatus.SubStatus[i].CurrAmount, selectedQuest.SubQuests[i].HuntInfo.Num);
			break;
		}
		case ESubQuestType::Collect:
		{
			const ItemInfo& info = gi->GetItemInfo(selectedQuest.SubQuests[i].CollectInfo.Index);
			str += FString::Printf(TEXT("%s ~ %d/%d"), *selectedQuest.SubQuests[i].Explanation, selectedStatus.SubStatus[i].CurrAmount, selectedQuest.SubQuests[i].CollectInfo.Num);
			break;
		}
		default:
			break;
		}

		if (selectedStatus.SubStatus[i].bCompleted == true)
		{
			str += "\t(Completed)";
		}
		str += "\n";

		explanationStr += str;
	}
	mSubQuestExplanationText->SetText(FText::FromString(explanationStr));

	mSelectedIndex = index;
}

void UQuestWidget::OnClicked_Exit()
{
	TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(pc);
	pc->CloseQuestTable(mOwnerPlayer);
}
