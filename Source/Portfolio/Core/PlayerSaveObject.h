#pragma once

#include "CoreMinimal.h"
#include "../../../ThirdParty/ProtoObject/SaveObject.pb.h"
#include "GameFramework/SaveGame.h"
#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "PlayerSaveObject.generated.h"

/*
	�÷��̾� ������ ���� ����ü
	- PlayerInfo : �� �̵� �� �÷��̾� ������ ������ �ӽ� �����ϱ� ���� �뵵
	- UPlayerSaveObject : ���̺� ����(.sav)�� ����/�ε��ϴ� �뵵
	- USaveSlot : ���̺� ������ ���ϸ�(�÷��̾��) ���
*/

struct PlayerInfo
{
	FString PlayerName;					// �÷��̾� �̸�
	int CurrentLevel;					// ���� ����
	int CurrentMap;						// ���� ��ġ�� �� �ε���
	FVector CurrentLocation;			// ���� �� ���� ��ġ
	int CurrentGold;					// ���� ��� ������
	FInventory Inventory;				// ���� �κ��丮
	TArray<FQuestStatus> QuestTable;	// ���� ����Ʈ ���൵

	int SlotIndex;						// ���̺� ���� ��ȣ

public:
	void ConvertFromProto(const SaveObject::PlayerInfo& info);
	void ConvertToProto(SaveObject::PlayerInfo& info);
};

UCLASS()
class PORTFOLIO_API USaveSlot : public USaveGame
{
	GENERATED_BODY()

public:
	USaveSlot();

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> mSlotNameList;
};


UCLASS()
class PORTFOLIO_API UPlayerSaveObject : public USaveGame
{
	GENERATED_BODY()

public:
	UPlayerSaveObject();

public:
	UPROPERTY(BlueprintReadWrite)
	FString mPlayerName;

	UPROPERTY(BlueprintReadWrite)
	int mCurrentLevel;

	UPROPERTY(BlueprintReadWrite)
	int mCurrentMap;

	UPROPERTY(BlueprintReadWrite)
	FVector mCurrentLocation;

	UPROPERTY(BlueprintReadWrite)
	int mCurrentGold;

	UPROPERTY(BlueprintReadWrite)
	FInventory mInventory;

	UPROPERTY(BlueprintReadWrite)
	TArray<FQuestStatus> mQuestTable;

public:
	UPROPERTY(BlueprintReadWrite)
	int mSlotIndex;
};
