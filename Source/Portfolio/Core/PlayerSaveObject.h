#pragma once

#include "CoreMinimal.h"
#include "../Network/ProtoObject/ProtoObject.pb.h"
#include "GameFramework/SaveGame.h"
#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "PlayerSaveObject.generated.h"

/*
	플레이어의 기본 정보
*/
struct PlayerInfo
{
	FString PlayerName;					// 닉네임
	int CurrentLevel;					// 레벨
	int CurrentMap;						// 현재 위치한 맵
	FVector CurrentLocation;			// 현재 위치
	int CurrentGold;					// 현재 골드 보유량
	FInventory Inventory;				// 인벤토리 상태
	TArray<FQuestStatus> QuestTable;	// 퀘스트 진행 상태
	
	int UserIdx;						// 유저 고유 번호

public:
	// ProtoBuf 구조체로부터 값 복사
	void ConvertFromProto(const ProtoObject::PlayerInfo& info);
	// ProtoBuf 구조체로 값 전달
	void ConvertToProto(ProtoObject::PlayerInfo& info) const;
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
