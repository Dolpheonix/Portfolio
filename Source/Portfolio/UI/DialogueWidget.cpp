// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "../Common/Dialogue.h"
#include "../Character/NpcCharacter.h"
#include "../Character/PlayerCharacter.h"
#include "../Core/CustomController.h"
#include "../Core/CustomPlayerState.h"
#include "TextButton.h"

static_assert(MAX_DIALOGUE_RESPONSE == 3);
static_assert(static_cast<uint8>(EDialogueEventType::Count) == 7);

UDialogueWidget::UDialogueWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
																				mRootCanvas(nullptr), mDialogueTextBlock(nullptr),
																				mOwnerNpc(nullptr), mOwnerPlayer(nullptr),
																				mCurrIndex(-1), bBounded(false)
{
	mResponseTextButton.SetNum(MAX_DIALOGUE_RESPONSE);
}

void UDialogueWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	mRootCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root")));

	mDialogueTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("DialogueText")));

	mResponseTextButton[0] = Cast<UTextButton>(GetWidgetFromName(TEXT("ResponseTextButton_0")));
	mResponseTextButton[1] = Cast<UTextButton>(GetWidgetFromName(TEXT("ResponseTextButton_1")));
	mResponseTextButton[2] = Cast<UTextButton>(GetWidgetFromName(TEXT("ResponseTextButton_2")));

	//TODO : 왜 이렇게 하면 안되는가?
	//for (int i = 0; i < MAX_DIALOGUE_RESPONSE; ++i)
	//{
	//	FString name = "ResponseTextButton_" + i;
	//	mResponseTextButton[i] = Cast<UTextButton>(GetWidgetFromName(FName(*name)));
	//}
}

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(mRootCanvas);
	check(mDialogueTextBlock);
	for (int i = 0; i < MAX_DIALOGUE_RESPONSE; ++i)
	{
		check(mResponseTextButton[i]);
	}

	SetIsFocusable(true);

	if (bBounded == false)
	{
		for (int i = 0; i < MAX_DIALOGUE_RESPONSE; ++i)
		{
			mResponseTextButton[i]->mClickEvent.AddDynamic(this, &UDialogueWidget::OnPressed_Response);
		}

		bBounded = true;
	}
}

void UDialogueWidget::Open(TObjectPtr<APlayerCharacter> player, TObjectPtr<ANpcCharacter> npc)
{
	check(player);
	check(npc);

	mOwnerPlayer = player;
	mOwnerNpc = npc;
	mCurrIndex = mOwnerNpc->GetDialogue().Bookmark;

	RenderDialogue();
}

void UDialogueWidget::Close()
{
	mOwnerNpc->UnInteract(mOwnerPlayer);
	mOwnerNpc = nullptr;
	mCurrIndex = -1;
}

void UDialogueWidget::RenderDialogue()
{
	const FDialogueLine& currDialogue = mOwnerNpc->GetDialogue().Dialogues[mCurrIndex];

	const FText text = FText::FromString(currDialogue.SpeakerName + ": " + currDialogue.Text);
	mDialogueTextBlock->SetText(text);
	
	for (int i = 0; i < MAX_DIALOGUE_RESPONSE; ++i)
	{
		const FDialogueResponse& response = currDialogue.Responses[i];
		if (response.Text.IsEmpty() == true)
		{
			mResponseTextButton[i]->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			mResponseTextButton[i]->SetVisibility(ESlateVisibility::Visible);
			mResponseTextButton[i]->mTextBlock->SetText(FText::FromString(response.Text));
		}
	}
}

void UDialogueWidget::OnPressed_Response(int i)
{
	check(i < MAX_DIALOGUE_RESPONSE);

	bool jumped = false;
	bool end = false;
	bool openShop = false;

	for (auto& event : mOwnerNpc->GetDialogue().Dialogues[mCurrIndex].Responses[i].Events)
	{
		switch (event.EventType)
		{
		case EDialogueEventType::End:
			end = true;
			break;
		case EDialogueEventType::Jump:
			jumped = true;
			mCurrIndex = event.JumpIndex;
			break;
		case EDialogueEventType::CommitQuest:
		{
			mOwnerPlayer->CommitQuest(event.QuestIndex);
			break;
		}
		case EDialogueEventType::CompleteQuest:
		{
			mOwnerPlayer->CompleteQuest(event.QuestIndex);
			break;
		}
		case EDialogueEventType::OpenShop:
			end = true;
			openShop = true;
			break;
		case EDialogueEventType::GiveItem:
		{
			check(event.ItemIndex < mOwnerNpc->GetRewardItems().Num());
			mOwnerPlayer->RootItem(mOwnerNpc->GetRewardItems()[event.ItemIndex]);
			break;
		}
		case EDialogueEventType::SetBookmark:
			mOwnerNpc->GetDialogue().Bookmark = event.BookmarkIndex;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Invalid EDialogueEventType"));
			break;
		}
	}
	
	if (end == true)
	{
		TObjectPtr<ACustomController> pc = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
		check(pc);
		pc->CloseDialogue(mOwnerPlayer, mOwnerNpc, openShop);
	}
	else
	{
		if (jumped == false)
		{
			++mCurrIndex;			
		}

		RenderDialogue();
	}
}
