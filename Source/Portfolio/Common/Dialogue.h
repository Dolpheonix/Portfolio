#pragma once

#include "CoreMinimal.h"
#include "Dialogue.generated.h"

constexpr int MAX_DIALOGUE_RESPONSE = 3; // 한 대화에서 선택할 수 있는 응답 개수


/*
	다이얼로그에서 응답을 선택했을 때, 발생하는 이벤트 종류
*/
UENUM(BlueprintType)
enum class EDialogueEventType : uint8
{
	End,			// 다이얼로그 종료. UI 창 닫음
	Jump,			// 특정한 인덱스로 다이얼로그 이동
	CommitQuest,	// 퀘스트를 등록함
	CompleteQuest,	// 퀘스트를 완료 처리함
	OpenShop,		// 상점 UI를 엶
	GiveItem,		// 아이템을 획득함
	SetBookmark,	// 북마크 설정. 다음 상호작용 시 해당 인덱스부터 다이얼로그를 시작함.
	Count,
};

/*
	다이얼로그 이벤트
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueEvent
{
	GENERATED_USTRUCT_BODY()

	EDialogueEventType EventType;
	union // 이벤트 타입 별로 필요한 변수
	{
		int JumpIndex;		// 점프할 다이얼로그의 인덱스
		int QuestIndex;		// 등록/완료할 퀘스트의 인덱스
		int ItemIndex;		// 획득할 아이템의 인덱스 (Npc 캐릭터의 mRewardItems 상의 인덱스)
		int BookmarkIndex;	// 다음 상호작용 시 시작 인덱스
	};

	// Json 객체로부터 정보를 받아옴
	bool LoadFromJson(const TSharedPtr<FJsonObject>& eventValue);

public:
	FDialogueEvent();
	FDialogueEvent(const FDialogueEvent& rhs) = default;
	FDialogueEvent& operator=(const FDialogueEvent& rhs) = default;
};

/*
	다이얼로그 응답 구조체
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueResponse
{
	GENERATED_USTRUCT_BODY()

	FString Text; // 응답 대사
	TArray<FDialogueEvent> Events;	// 해당 응답을 선택했을 때 발생할 이벤트 배열

	// Json 객체로부터 정보를 받아옴
	bool LoadFromJson(const TSharedPtr<FJsonObject>& responseValue);

public:
	FDialogueResponse();
	FDialogueResponse(const FDialogueResponse& rhs) = default;
	FDialogueResponse& operator=(const FDialogueResponse& rhs) = default;
};

/*
	다이얼로그의 기본 단위
	- 대사
	- 응답 1~3
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueLine
{
	GENERATED_USTRUCT_BODY()

	FString SpeakerName;	// 화자 이름
	FString Text;			// 대사
	TArray<FDialogueResponse> Responses;	// 선택 가능한 응답

	// Json 객체로부터 정보를 받아옴
	bool LoadFromJson(const TSharedPtr<FJsonObject>& lineValue);

public:
	FDialogueLine();
	FDialogueLine(const FDialogueLine& rhs) = default;
	FDialogueLine& operator=(const FDialogueLine& rhs) = default;
};

/*
	다이얼로그 리스트
	- 다이얼로그 배열
	- 북마크
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueList
{
	GENERATED_USTRUCT_BODY()

	TArray<FDialogueLine> Dialogues;
	int Bookmark;	// 북마크

	// Json 객체로부터 정보를 받아옴
	bool LoadFromJson(const TSharedPtr<FJsonObject>& listValue);

public:
	FDialogueList();
	FDialogueList(const FDialogueList& rhs) = default;
	FDialogueList& operator=(const FDialogueList& rhs) = default;
};