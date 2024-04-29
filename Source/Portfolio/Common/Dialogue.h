#pragma once

#include "CoreMinimal.h"
#include "Dialogue.generated.h"

// �� ��ȭ�� ���� ������ ������ �ִ� ����
constexpr int MAX_DIALOGUE_RESPONSE = 3;


/*
	EDialogueEventType : ��ȭ���� Ư�� ������ �������� �� �߻��ϴ� �̺�Ʈ Ÿ��
		- End : ��ȭ ���� (UI â �ݱ�)
		- Jump : Ư�� �ε����� ��ȭ�� �̵�
		- Commit Quest : ����Ʈ�� ����
		- Complete Quest : ����Ʈ�� �Ϸ� ó����
		- Open Shop : ���� UI�� ��
		- Give Item : �������� ��
		- Set Bookmark : ������ �ش� npc�� ��ȭ�� �����ϸ� �ش� �ε������� ����
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
	FDialogueEvent : ��ȭ �̺�Ʈ ����ü
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueEvent
{
	GENERATED_USTRUCT_BODY()

	EDialogueEventType EventType;
	union // EventType�� ���� �ٸ� ���� ���
	{
		int JumpIndex;		// Jump�� �� ��� ==> ������ �ε���
		int QuestIndex;		// CommitQuest/CompleteQuest ==> ����Ʈ �ε���
		int ItemIndex;		// GiveItem ==> �� ������ �ε��� (NPC ĳ������ RewardItems �迭 ���� �ε���)
		int BookmarkIndex;	// SetBookmark ==> ��ȭ �ϸ�ũ
	};

	bool LoadFromJson(const TSharedPtr<FJsonObject>& eventValue);

public:
	FDialogueEvent();
	FDialogueEvent(const FDialogueEvent& rhs) = default;
	FDialogueEvent& operator=(const FDialogueEvent& rhs) = default;
};

/*
	FDialogueResponse : ��ȭ�� ���� ����. �̺�Ʈ�� ����
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueResponse
{
	GENERATED_USTRUCT_BODY()

	FString Text;	// ���� �ؽ�Ʈ
	TArray<FDialogueEvent> Events;	// �̺�Ʈ�� ���� ���� �߻��� �� ����.

	bool LoadFromJson(const TSharedPtr<FJsonObject>& responseValue);

public:
	FDialogueResponse();
	FDialogueResponse(const FDialogueResponse& rhs) = default;
	FDialogueResponse& operator=(const FDialogueResponse& rhs) = default;
};

/*
	FDialogueLine : ��� + ������ ������ �� ���̾�α�
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueLine
{
	GENERATED_USTRUCT_BODY()

	FString SpeakerName;	// ȭ��
	FString Text;			// ��� �ؽ�Ʈ
	TArray<FDialogueResponse> Responses;	// ���� ������ ����

	bool LoadFromJson(const TSharedPtr<FJsonObject>& lineValue);

public:
	FDialogueLine();
	FDialogueLine(const FDialogueLine& rhs) = default;
	FDialogueLine& operator=(const FDialogueLine& rhs) = default;
};

/*
	FDialogueList : ���̾�α� ����Ʈ. NPC ĳ���Ͱ� ������ ���� ����ü
*/

USTRUCT(Atomic, BlueprintType)
struct FDialogueList
{
	GENERATED_USTRUCT_BODY()

	TArray<FDialogueLine> Dialogues;
	int Bookmark;	// ���� ��ȭ�� �ϸ�ũ

	bool LoadFromJson(const TSharedPtr<FJsonObject>& listValue);

public:
	FDialogueList();
	FDialogueList(const FDialogueList& rhs) = default;
	FDialogueList& operator=(const FDialogueList& rhs) = default;
};