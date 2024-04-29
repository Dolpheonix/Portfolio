#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "PlayerSaveObject.generated.h"

/*
	플레이어 정보를 담은 구조체
	- PlayerInfo : 맵 이동 시 플레이어 액터의 정보를 임시 저장하기 위한 용도
	- UPlayerSaveObject : 세이브 파일(.sav)에 저장/로드하는 용도
	- USaveSlot : 세이브 슬롯의 파일명(플레이어명) 목록
*/

struct PlayerInfo
{
	FString PlayerName;					// 플레이어 이름
	int CurrentLevel;					// 현재 레벨
	int CurrentMap;						// 현재 위치한 맵 인덱스
	FVector CurrentLocation;			// 현재 맵 상의 위치
	int CurrentGold;					// 현재 골드 보유량
	FInventory Inventory;				// 현재 인벤토리
	TArray<FQuestStatus> QuestTable;	// 현재 퀘스트 진행도

	int SlotIndex;						// 세이브 슬롯 번호
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
