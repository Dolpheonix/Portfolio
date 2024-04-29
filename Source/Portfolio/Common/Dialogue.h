#pragma once

#include "CoreMinimal.h"
#include "Dialogue.generated.h"

// 한 대화에 선택 가능한 응답의 최대 개수
constexpr int MAX_DIALOGUE_RESPONSE = 3;


/*
	EDialogueEventType : 대화에서 특정 응답을 선택했을 때 발생하는 이벤트 타입
		- End : 대화 종료 (UI 창 닫기)
		- Jump : 특정 인덱스로 대화를 이동
		- Commit Quest : 퀘스트를 맡음
		- Complete Quest : 퀘스트를 완료 처리함
		- Open Shop : 상점 UI를 엶
		- Give Item : 아이템을 줌
		- Set Bookmark : 다음에 해당 npc와 대화를 시작하면 해당 인덱스부터 시작
*/
UENUM(BlueprintType)
enum class EDialogueEventType : uint8
{
	End,
	Jump,
	CommitQuest,
	CompleteQuest,
	OpenShop,
	GiveItem,
	SetBookmark,
	Count,
};

/*
	FDialogueEvent : 대화 이벤트 구조체
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueEvent
{
	GENERATED_USTRUCT_BODY()

	EDialogueEventType EventType;
	union // EventType에 따라 다른 변수 사용
	{
		int JumpIndex;		// Jump일 때 사용 ==> 점프할 인덱스
		int QuestIndex;		// CommitQuest/CompleteQuest ==> 퀘스트 인덱스
		int ItemIndex;		// GiveItem ==> 줄 아이템 인덱스 (NPC 캐릭터의 RewardItems 배열 상의 인덱스)
		int BookmarkIndex;	// SetBookmark ==> 대화 북마크
	};

	bool LoadFromJson(const TSharedPtr<FJsonObject>& eventValue);

public:
	FDialogueEvent();
	FDialogueEvent(const FDialogueEvent& rhs) = default;
	FDialogueEvent& operator=(const FDialogueEvent& rhs) = default;
};

/*
	FDialogueResponse : 대화에 따른 응답. 이벤트를 포함
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueResponse
{
	GENERATED_USTRUCT_BODY()

	FString Text;	// 응답 텍스트
	TArray<FDialogueEvent> Events;	// 이벤트는 여러 개가 발생할 수 있음.

	bool LoadFromJson(const TSharedPtr<FJsonObject>& responseValue);

public:
	FDialogueResponse();
	FDialogueResponse(const FDialogueResponse& rhs) = default;
	FDialogueResponse& operator=(const FDialogueResponse& rhs) = default;
};

/*
	FDialogueLine : 대사 + 응답을 포함한 한 다이얼로그
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueLine
{
	GENERATED_USTRUCT_BODY()

	FString SpeakerName;	// 화자
	FString Text;			// 대사 텍스트
	TArray<FDialogueResponse> Responses;	// 선택 가능한 응답

	bool LoadFromJson(const TSharedPtr<FJsonObject>& lineValue);

public:
	FDialogueLine();
	FDialogueLine(const FDialogueLine& rhs) = default;
	FDialogueLine& operator=(const FDialogueLine& rhs) = default;
};

/*
	FDialogueList : 다이얼로그 리스트. NPC 캐릭터가 가지고 있을 구조체
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueList
{
	GENERATED_USTRUCT_BODY()

	TArray<FDialogueLine> Dialogues;
	int Bookmark;	// 현재 대화의 북마크

	bool LoadFromJson(const TSharedPtr<FJsonObject>& listValue);

public:
	FDialogueList();
	FDialogueList(const FDialogueList& rhs) = default;
	FDialogueList& operator=(const FDialogueList& rhs) = default;
};