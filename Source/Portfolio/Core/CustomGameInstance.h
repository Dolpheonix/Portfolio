#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <thread>

#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "../Common/Enemy.h"
#include "PlayerSaveObject.h"

#include "CustomGameInstance.generated.h"

constexpr int MAX_CHATTING_TO_SHOW = 20;
constexpr int MAX_SAVE_SLOT = 10;
constexpr int MAX_QUEST_COMMIT = 10;

class APlayerCharacter;
class ChattingClient;

/*
	UCustomGameInstance : 기본 게임 인스턴스
		- 세이브 파일 저장/로드
		- 게임 데이터 로드
		- 맵 데이터 로드 (NPC, Enemy 데이터)
*/
UCLASS()
class PORTFOLIO_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UCustomGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

public:
	void OpenIntro();

	// 게임 데이터 로딩
	void LoadGameData();
	void OnMapLoaded(UWorld* world);

// On Map Loaded
	// Npc 데이터 로드
	void LoadNpcList(UWorld* world);
	// Enemy 데이터 로드/스폰
	void LoadEnemyList(UWorld* world);

	// Save/Load
	int CreateSaveFile(FString playerName);
	void SaveGame(TObjectPtr<APlayerCharacter> player);
	void LoadGame(int slotIndex);

	// Getter/Setter
	bool IsLoaded();
	void SetIsIntro(bool b);
	bool IsIntro();
	const TArray<ItemInfo>& GetItemInfoList() const;
	const ItemInfo& GetItemInfo(int index) const;
	const TArray<FQuest>& GetQuestInfoList() const;
	const FQuest& GetQuestInfo(int index) const;
	const TArray<FEnemyInfo>& GetEnemyInfoList() const;
	const FEnemyInfo& GetEnemyInfo(int index) const;
	const TArray<FEnemyLabelInfo>& GetEnemyLabelInfoList() const;
	const FEnemyLabelInfo& GetEnemyLabelInfo(int index) const;
	const FString& GetLevelName(int index) const;
	const TArray<FString>& GetLevelList() const;
	void SetPlayerInfo(const PlayerInfo& info);
	PlayerInfo GetPlayerInfo() const;
	PlayerInfo GetSaveData(int slotIndex) const;
	const TArray<FString>& GetSavedChattings() const;

	void SendChatMessage(const char* chat);
	void HandleChatMessage(const char* chat);

private:
	TArray<ItemInfo> mItemInfoList;
	TArray<FQuest> mQuestInfoList;
	TArray<FEnemyInfo> mEnemyInfoList;
	TArray<FEnemyLabelInfo> mEnemyLabelInfoList;
	TArray<FString> mLevelList;

	PlayerInfo mTempPlayerInfo; // 맵 이동 시 플레이어 정보를 임시로 저장하는 용도

	ChattingClient* mChattingClient;
	std::thread mChattingThread;
	TArray<FString> mSavedChattings;

	bool bLoaded;
	bool bIntro;
};
