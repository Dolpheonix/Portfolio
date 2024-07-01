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

	// TEMP : 인덱스를 %d로 넘겨 for문을 돌리는 방식이 제대로 동작하지 않음
	mResponseTextButton[0] = Cast<UTextButton>(GetWidgetFromName(TEXT("ResponseTextButton_0")));
	mResponseTextButton[1] = Cast<UTextButton>(GetWidgetFromName(TEXT("ResponseTextButton_1")));
	mResponseTextButton[2] = Cast<UTextButton>(GetWidgetFromName(TEXT("ResponseTextButton_2")));
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
	mCurrIndex = mOwnerNpc->GetDialogue().Bookmark;	// 북마크부터 다이얼로그를 시작함

	RenderDialogue();
}

void UDialogueWidget::Close()
{
	// 변수 초기화
	mOwnerNpc->UnInteract(mOwnerPlayer);
	mOwnerNpc = nullptr;
	mCurrIndex = -1;
}

void UDialogueWidget::RenderDialogue()
{
	const FDialogueLine& currDialogue = mOwnerNpc->GetDialogue().Dialogues[mCurrIndex];

	// "Speaker : HelloWorld"
	const FText text = FText::FromString(currDialogue.SpeakerName + ": " + currDialogue.Text);
	mDialogueTextBlock->SetText(text);
	
	for (int i = 0; i < MAX_DIALOGUE_RESPONSE; ++i)
	{
		const FDialogueResponse& response = currDialogue.Responses[i];
		// 응답 텍스트가 비어있으면 없는 걸로 취급하고, 버튼도 비활성화
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

	// 함수 끝에 처리할 이벤트는 boolean 값에 담아둠
	bool jumped = false;	// 특정 인덱스로 점프
	bool end = false;		// 대화 UI를 종료
	bool openShop = false;	// 상점 UI를 엶

	for (auto& event : mOwnerNpc->GetDialogue().Dialogues[mCurrIndex].Responses[i].Events)
	{
		switch (event.EventType)
		{
		case EDialogueEventType::End:
			end = true;
			break;
		case EDialogueEventType::Jump:
			jumped = true;
			mCurrIndex = event.JumpIndex;	// 점프할 인덱스로 바꿔놓음
			break;
		case EDialogueEventType::CommitQuest:
		{
			mOwnerPlayer->CommitQuest(event.QuestIndex);	// 퀘스트를 등록
			break;
		}
		case EDialogueEventType::CompleteQuest:
		{
			mOwnerPlayer->CompleteQuest(event.QuestIndex);	// 퀘스트를 완료
			break;
		}
		case EDialogueEventType::OpenShop:
			end = true;
			openShop = true;
			break;
		case EDialogueEventType::GiveItem:
		{
			check(event.ItemIndex < mOwnerNpc->GetRewardItems().Num());
			mOwnerPlayer->RootItem(mOwnerNpc->GetRewardItems()[event.ItemIndex]);	// 아이템 획득
			break;
		}
		case EDialogueEventType::SetBookmark:
			mOwnerNpc->GetDialogue().Bookmark = event.BookmarkIndex;	// 북마크 설정
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
		// 점프 이벤트가 없다면 다음 인덱스로 넘긴다.
		if (jumped == false)
		{
			++mCurrIndex;			
		}

		RenderDialogue();
	}
}
