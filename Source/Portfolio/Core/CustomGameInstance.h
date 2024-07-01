#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <thread>
#include "PlayerSaveObject.h"
#include "../Common/Item.h"
#include "../Common/Quest.h"
#include "../Common/Enemy.h"
#include "CustomGameInstance.generated.h"

constexpr int MAX_CHATTING_TO_SHOW = 20;
constexpr int MAX_SAVE_SLOT = 10;
constexpr int MAX_QUEST_COMMIT = 10;

class APlayerCharacter;
class AReplicatedCharacter;
class AEnemyCharacter;
class ARootItem;
class MainClient;
class ChattingClient;

/*
	게임 인스턴스
		1. 게임 데이터 로딩
		2. 클라이언트와 인게임 오브젝트의 매개체 역할
		3. 맵 이동, 게임 시작/종료 등을 처리
*/
UCLASS()
class PORTFOLIO_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UCustomGameInstance();

	// 게임 데이터 로드, 서버 연결 등의 작업 처리
	virtual void Init() override;
	// 서버와 연결 해제
	virtual void Shutdown() override;

public:
	// 인트로 화면으로 전환
	void OpenIntro();
	// 게임 데이터 로딩
	void LoadGameData();
	// Map 전환 시 호출되는 함수. 하단 함수들 묶음
	void OnMapLoaded(UWorld* world);
	// 맵 내의 NPC에 데이터 적용 (다이얼로그, 아이템)
	void LoadNpcList(UWorld* world);	
	// 맵 내의 스폰 지점에 Enemy 스폰
	void LoadEnemyList(UWorld* world);	
	// 맵 내의 스폰 지점에 RootItem 스폰
	void LoadRootItemList(UWorld* world);

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
	const TArray<FString>& GetSavedChattings() const;

	// Chatting Handler
	// 서버에 채팅 메시지 전송
	void SendChatMessage(const char* chat);		
	// 서버로부터 전송받은 메시지 처리
	void HandleChatMessage(const char* chat);	

	// 인트로에서 게임 시작
	void StartGame();
	// mapIdx 번째 맵을 로드
	void LoadMap(uint32 mapIdx);

	// Login Handler
	// 로그인 시도
	bool TryLogin(const char* id, const char* pw);		
	// 회원가입 시도
	bool TryRegister(const char* id, const char* pw);	
	// 닉네임 설정
	bool SetNickname(const char* nickname);				

	// InGame Network Method
	// 현재 위치, 회전 상태를 전송
	void SendLocation(FVector loc, FRotator rot);
	// 점프 여부를 전송
	void SendJumping(bool isJumping);
	// 뛰기/걷기 여부 전송
	void SendRunning(bool isRunning);
	// 장비 장착/해제 여부 전송
	void SendEquipping(bool isEquipping);
	// 의상/무기 장착이 변경됨을 전송
	void SendEquipmentChange(EItemType type, uint64 infoIndex);
	// 맵에서 몹이 삭제됨을 전송
	void SendEnemyRemoval(uint64 resIdx);
	// 맵에서 아이템이 삭제됨을 전송
	void SendItemRemoval(uint64 resIdx);
	// 플레이어가 맵을 이탈/진입함을 전송
	void SendMapTransition(uint32 mapBefore, uint32 mapAfter);

	// 특정 유저의 위치/회전 정보를 받음
	void ReplicateLocation(uint64 userIdx, FVector loc, FRotator rot);
	// 특정 유저의 점프 여부 정보를 받음
	void ReplicateJumping(uint64 userIdx, bool isJumping);
	// 특정 유저의 걷기/뛰기 여부 정보를 받음
	void ReplicateRunning(uint64 userIdx, bool isRunning);
	// 특정 유저의 무기 장착/해제 여부 정보를 받음
	void ReplicateEquipping(uint64 userIdx, bool isEquipping);
	// 특정 유저의 의상/무기 장착이 변경된 정보를 받음
	void ReplicateEquipmentChange(uint64 userIdx, EItemType type, uint64 infoIndex);
	// 맵에서 몹이 삭제된 정보를 받음
	void ReplicateEnemyRemoval(uint32 mapIdx, uint64 resIdx);
	// 맵에서 아이템이 삭제된 정보를 받음
	void ReplicateItemRemoval(uint32 mapIdx, uint64 resIdx);
	// 특정 유저가 맵에 이탈/진입한 정보를 받음
	void ReplicateMapTransition(uint64 userIdx, uint32 mapBefore, uint32 mapAfter);

private:
	TArray<ItemInfo> mItemInfoList;					// 아이템 정보 리스트 (게임 데이터)
	TArray<FQuest> mQuestInfoList;					// 퀘스트 정보 리스트 (게임 데이터)
	TArray<FEnemyInfo> mEnemyInfoList;				// 몹 정보 리스트	 (게임 데이터)
	TArray<FEnemyLabelInfo> mEnemyLabelInfoList;	// 몹 라벨 리스트	 (게임 데이터)
	TArray<FString> mLevelList;						// 레벨 리스트 (게임 데이터)

	PlayerInfo mTempPlayerInfo;			// 캐릭터, 서버에 데이터를 저장/로드하기 전에 임시로 담아두는 용도

	TMap<uint64, TObjectPtr<AReplicatedCharacter>> mReplicatedCharacterList;
	TMap<uint64, TObjectPtr<AEnemyCharacter>> mEnemyCharacterList;
	TMap<uint64, TObjectPtr<ARootItem>> mRootItemList;


	ChattingClient* mChattingClient;	// 채팅 서버와 연결할 클라이언트
	std::thread mChattingThread;		// 채팅 클라이언트용 스레드
	TArray<FString> mSavedChattings;	// 서버로부터 받은 채팅 목록 (채팅창에 표시할 만큼만 담음)

	MainClient* mMainClient;			// 게임 서버와 연결할 클라이언트
	std::thread mMainThread;			// 게임 클라이언트용 스레드

	// boolean 모음
	bool bLoaded;	// 게임 데이터가 모두 로드된 상태인가?
	bool bIntro;	// 현재 인트로가 열린 상태인가?
};
