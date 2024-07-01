#include "CustomGameInstance.h"
#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "../Actor/RootItem.h"
#include "../Character/PlayerCharacter.h"
#include "../Character/EnemyCharacter.h"
#include "../Character/NpcCharacter.h"
#include "../Character/ReplicatedCharacter.h"
#include "../Network/MainClient.h"
#include "../Network/ChattingClient.h"
#include "../Base/Func.h"
#include "../Base/LoadHelper.h"
#include "../Common/GameObject.h"
#include "CustomController.h"

using GameObject::RepBoolean::RepType::JUMPING;
using GameObject::RepBoolean::RepType::RUNNING;
using GameObject::RepBoolean::RepType::EQUIPPED;
using GameObject::Equipment::EquipmentType::CLOTH;
using GameObject::Equipment::EquipmentType::WEAPON;
using GameObject::ResourceChange::ResourceType::ENEMY;
using GameObject::ResourceChange::ResourceType::ITEM;
using GameObject::ResourceChange::ChangeType::SPAWN;
using GameObject::ResourceChange::ChangeType::REMOVE;

UCustomGameInstance::UCustomGameInstance() : UGameInstance(), bLoaded(false), bIntro(false),
											 mMainClient(nullptr), mChattingClient(nullptr)
{
	mSavedChattings.SetNum(MAX_CHATTING_TO_SHOW);	// 채팅창에 최대 20개의 채팅만 보여줌
}

void UCustomGameInstance::Init()
{
	if (bLoaded == false)
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UCustomGameInstance::OnMapLoaded);
		LoadGameData();	// 게임 데이터 로딩

		// 채팅 서버용 WSA
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Can't start WSA"));
		}

		// 게임 서버와 연결
		mMainClient = MainClient::GetSingleton();
		check(mMainClient);
		// TEMP : 게임 스레드 포함 언리얼 제공 스레드 사용 시에 크래시가 발생함.
		mMainThread = thread(&MainClient::RunClient, mMainClient, this);

		// 채팅 서버와 연결
		mChattingClient = ChattingClient::GetSingleton();
		check(mChattingClient);
		if (mChattingClient->Init(this) == SOCKET_ERROR)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Can't init Chatting Client"));
		}

		mChattingThread = thread(&ChattingClient::Run, mChattingClient);	// 채팅 송신용 스레드

		bLoaded = true;
	}

	bIntro = true;	// 첫 화면은 인트로

	UGameInstance::Init();
}

void UCustomGameInstance::Shutdown()
{
	// 종료 직전의 게임 데이터를 저장
	if (bIntro == false)
	{
		mMainClient->Save(mTempPlayerInfo);	// 서버에 현재 상태를 저장
	}

	// 메인 스레드 종료
	mMainThread.join();

	// 채팅 스레드 종료
	mChattingClient->SetRunning(false);
	mChattingThread.join();

	WSACleanup();

	UGameInstance::Shutdown();
}

void UCustomGameInstance::OpenIntro()
{
	bIntro = true;
	UGameplayStatics::OpenLevel(this, TEXT("Test_Intro"));	// 인트로용 빈 맵 로딩
}

void UCustomGameInstance::LoadGameData()
{
	// Content/Data/ 내의 게임 데이터 로딩
	ItemInfo::LoadInfoListFromJson(mItemInfoList);
	FQuest::LoadInfoListFromJson(mQuestInfoList);
	FEnemyLabelInfo::LoadInfoListFromJson(mEnemyLabelInfoList);
	FEnemyInfo::LoadInfoListFromJson(mEnemyInfoList);

	// 레벨 리스트 로딩
	FString result;
	const FString listPath = FPaths::ProjectContentDir() + "Data/LevelInfo.json";

	if (FFileHelper::LoadFileToString(result, *listPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("LevelInfo not loaded"));
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	const TArray<TSharedPtr<FJsonValue>>& levelInfoList = root->GetArrayField("LevelInfoList");
	mLevelList.SetNum(levelInfoList.Num());
	for (int i = 0; i < levelInfoList.Num(); ++i)
	{
		mLevelList[i] = levelInfoList[i]->AsObject()->GetStringField("Name");
	}
}

void UCustomGameInstance::OnMapLoaded(UWorld* world)
{
	const FString& currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
	if (currentLevelName == "Test_Intro")
	{
		return;
	}

	mReplicatedCharacterList.Reset();
	mRootItemList.Reset();
	mEnemyCharacterList.Reset();

	// NPC, 몹, 아이템 로딩
	LoadNpcList(world);
	LoadEnemyList(world);
	LoadRootItemList(world);
}

void UCustomGameInstance::LoadNpcList(UWorld* world)
{
	// Ex) Content/Data/LevelData/Tutorial/Npc.json
	const FString& currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
	const FString levelDataPath = FPaths::ProjectContentDir() + "Data/LevelData/" + currentLevelName + "/";
	const FString npcDataPath = levelDataPath + "Npc.json";

	FString result;
	if (FFileHelper::LoadFileToString(result, *npcDataPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("NpcData not loaded"));
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	// 레벨 내의 NPC 액터 리스트
	TArray<TObjectPtr<AActor>> npcList;
	UGameplayStatics::GetAllActorsOfClass(this, ANpcCharacter::StaticClass(), npcList);

	const TArray<TSharedPtr<FJsonValue>>& npcInfoList = root->GetArrayField(TEXT("NpcList"));
	for (auto& npcInfo : npcInfoList)
	{
		const TSharedPtr<FJsonObject>& npcObject = npcInfo->AsObject();
		const int npcIndex = npcObject->GetIntegerField("Index");

		// npcIndex에 맞는 레벨 내 액터를 찾아야 함
		auto foundNpc = npcList.FindByPredicate([npcIndex](const TObjectPtr<AActor> actor)
			{
				return (Cast<ANpcCharacter>(actor)->GetIndex() == npcIndex);
			}
		);

		if (!foundNpc)
		{
			UE_LOG(LogTemp, Log, TEXT("해당 NPC가 존재하지 않습니다. [%s]"), *npcObject->GetStringField("Name"));
			continue;
		}

		TObjectPtr<ANpcCharacter> npc = Cast<ANpcCharacter>(foundNpc->Get());
		npc->LoadFromJson(npcObject);	// json 데이터를 NPC 액터에 적용
	}
}

void UCustomGameInstance::LoadEnemyList(UWorld* world)
{
	// Ex) Content/Data/LevelData/Tutorial/Enemy.json
	const FString& currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
	const FString levelDataPath = FPaths::ProjectContentDir() + "Data/LevelData/" + currentLevelName + "/";
	const FString enemyDataPath = levelDataPath + "Enemy.json";

	FString result;
	if (FFileHelper::LoadFileToString(result, *enemyDataPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("EnemyData not loaded"));
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	// NOTE : 몹은 NPC와 달리 클래스 별로 외형이 정형화되어 있기 때문에, 에디터로 디자인하지 않고 맵 로딩 시에 스폰한다.
	const TArray<TSharedPtr<FJsonValue>>& enemyValueList = root->GetArrayField(TEXT("EnemyList"));
	for (auto& enemyValue : enemyValueList)
	{
		const TSharedPtr<FJsonObject>& enemyObject = enemyValue->AsObject();

		const FVector spawnLocation = Helper::JsonToVector(enemyObject->GetArrayField("SpawnLocation")); // 스폰 위치
		const FEnemyInfo& enemyInfo = GetEnemyInfo(enemyObject->GetIntegerField("InfoIndex"));	// 몹 정보

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = world->GetLevelScriptActor();
		spawnParam.Instigator = world->GetLevelScriptActor()->GetInstigator();
		
		// 몹 스폰
		TObjectPtr<AEnemyCharacter> spawnedEnemy = world->SpawnActor<AEnemyCharacter>(enemyInfo.EnemyClass, spawnLocation, FRotator(), spawnParam);
		if (spawnedEnemy)
		{
			spawnedEnemy->SetResourceIndex(enemyObject->GetIntegerField("Index"));
			spawnedEnemy->SetEnemyInfoIndex(enemyObject->GetIntegerField("InfoIndex"));

			mEnemyCharacterList.Add(enemyObject->GetIntegerField("Index"), spawnedEnemy);
			spawnedEnemy->SpawnDefaultController();
		}
	}
}

void UCustomGameInstance::LoadRootItemList(UWorld* world)
{
	// Ex) Content/Data/LevelData/Tutorial/Enemy.json
	const FString& currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
	const FString levelDataPath = FPaths::ProjectContentDir() + "Data/LevelData/" + currentLevelName + "/";
	const FString enemyDataPath = levelDataPath + "Item.json";

	FString result;
	if (FFileHelper::LoadFileToString(result, *enemyDataPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("RootItemData not loaded"));
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	// NOTE : 몹은 NPC와 달리 클래스 별로 외형이 정형화되어 있기 때문에, 에디터로 디자인하지 않고 맵 로딩 시에 스폰한다.
	const TArray<TSharedPtr<FJsonValue>>& itemValueList = root->GetArrayField(TEXT("ItemList"));
	for (auto& itemValue : itemValueList)
	{
		const TSharedPtr<FJsonObject>& itemObject = itemValue->AsObject();

		const FVector spawnLocation = Helper::JsonToVector(itemObject->GetArrayField("SpawnLocation")); // 스폰 위치

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = world->GetLevelScriptActor();
		spawnParam.Instigator = world->GetLevelScriptActor()->GetInstigator();

		// 몹 스폰
		TObjectPtr<ARootItem> spawnedItem = world->SpawnActor<ARootItem>(ARootItem::StaticClass(), spawnLocation, FRotator(), spawnParam);
		if (spawnedItem)
		{
			FGameItem rootInfo(itemObject->GetIntegerField("InfoIndex"), itemObject->GetIntegerField("Num"));
			spawnedItem->SetItem(rootInfo);
			spawnedItem->SetResourceIndex(itemObject->GetIntegerField("Index"));
			mRootItemList.Add(itemObject->GetIntegerField("Index"), spawnedItem);
		}
	}
}

bool UCustomGameInstance::IsLoaded()
{
	return bLoaded;
}

void UCustomGameInstance::SetIsIntro(bool b)
{
	bIntro = b;
}

bool UCustomGameInstance::IsIntro()
{
	return bIntro;
}

const TArray<ItemInfo>& UCustomGameInstance::GetItemInfoList() const
{
	return mItemInfoList;
}

const ItemInfo& UCustomGameInstance::GetItemInfo(int index) const
{
	check(index < mItemInfoList.Num());
	return mItemInfoList[index];
}

const TArray<FQuest>& UCustomGameInstance::GetQuestInfoList() const
{
	return mQuestInfoList;
}

const FQuest& UCustomGameInstance::GetQuestInfo(int index) const
{
	check(index < mQuestInfoList.Num());
	return mQuestInfoList[index];
}

const TArray<FEnemyInfo>& UCustomGameInstance::GetEnemyInfoList() const
{
	return mEnemyInfoList;
}

const FEnemyInfo& UCustomGameInstance::GetEnemyInfo(int index) const
{
	check(index < mEnemyInfoList.Num());
	return mEnemyInfoList[index];
}

const TArray<FEnemyLabelInfo>& UCustomGameInstance::GetEnemyLabelInfoList() const
{
	return mEnemyLabelInfoList;
}

const FEnemyLabelInfo& UCustomGameInstance::GetEnemyLabelInfo(int index) const
{
	check(index < mEnemyLabelInfoList.Num());
	return mEnemyLabelInfoList[index];
}

const FString& UCustomGameInstance::GetLevelName(int index) const
{
	check(index < mLevelList.Num());
	return mLevelList[index];
}

const TArray<FString>& UCustomGameInstance::GetLevelList() const
{
	return mLevelList;
}

void UCustomGameInstance::SetPlayerInfo(const PlayerInfo& info)
{
	mTempPlayerInfo = info;
}

PlayerInfo UCustomGameInstance::GetPlayerInfo() const
{
	return mTempPlayerInfo;
}

const TArray<FString>& UCustomGameInstance::GetSavedChattings() const
{
	return mSavedChattings;
}

void UCustomGameInstance::SendChatMessage(const char* chat)
{
	mChattingClient->SendChatting(chat);
}

void UCustomGameInstance::HandleChatMessage(const char* chat)
{
	for (int i = MAX_CHATTING_TO_SHOW - 1; i > 0; --i)
	{
		mSavedChattings[i] = mSavedChattings[i - 1];	// 기존 채팅은 한칸씩 뒤로 밂
	}

	mSavedChattings[0] = chat;

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->UpdateChatting();	// 채팅창 업데이트
}

void UCustomGameInstance::StartGame()
{
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->CloseIntro();	// 인트로 UI를 제거

	bIntro = false;

	// 채팅 서버에 닉네임 전송 (채팅 시에 닉네임 : '...' 으로 표시되어야 함.)
	const char* msg = TCHAR_TO_ANSI(*mTempPlayerInfo.PlayerName);
	mChattingClient->SetName(msg);

	UGameplayStatics::OpenLevel(this, FName(mLevelList[mTempPlayerInfo.CurrentMap]));	// 맵 로딩
}

void UCustomGameInstance::LoadMap(uint32 mapIdx)
{
	// 서버에 맵 이탈/진입을 알림
	SendMapTransition(mTempPlayerInfo.CurrentMap, mapIdx);

	mTempPlayerInfo.CurrentMap = mapIdx;
	UGameplayStatics::OpenLevel(this, FName(mLevelList[mapIdx]));
}

bool UCustomGameInstance::TryLogin(const char* id, const char* pw)
{
	PlayerInfo loginResult = mMainClient->TryLogin(id, pw);
	if (loginResult.CurrentMap == 10000)
	{
		return false;	// 로그인 실패
	}

	// 로그인이 성공했다면, 받은 데이터로 게임 시작
	mTempPlayerInfo = loginResult;
	StartGame();

	return true;
}

bool UCustomGameInstance::TryRegister(const char* id, const char* pw)
{
	PlayerInfo registerResult = mMainClient->TryRegister(id, pw);
	if (registerResult.CurrentMap == 10000)
	{
		return false;	// 회원가입 실패
	}

	// NOTE : '회원가입 성공 --> 닉네임 설정 --> 게임 시작' 이므로, StartGame은 여기서 안불림.
	mTempPlayerInfo = registerResult;

	return true;
}

bool UCustomGameInstance::SetNickname(const char* nickname)
{
	string submitResult = mMainClient->SetNickname(nickname);
	if (submitResult == "failed")
	{
		return false;	// 닉네임 설정 실패
	}

	// 닉네임 설정 성공 --> 게임 시작
	mTempPlayerInfo.PlayerName = submitResult.c_str();
	StartGame();

	return true;
}

void UCustomGameInstance::SendLocation(FVector loc, FRotator rot)
{
	GameObject::Location req;
	req.UserIdx = mTempPlayerInfo.UserIdx;
	req.CurrLocation = loc;
	req.CurrRotation = rot;

	mMainClient->SendLocation(req);
}

void UCustomGameInstance::SendJumping(bool isJumping)
{
	GameObject::RepBoolean req;
	req.UserIdx = mTempPlayerInfo.UserIdx;
	req.Type = JUMPING;
	req.Boolean = isJumping;

	mMainClient->SendRepBoolean(req);
}

void UCustomGameInstance::SendRunning(bool isRunning)
{
	GameObject::RepBoolean req;
	req.UserIdx = mTempPlayerInfo.UserIdx;
	req.Type = RUNNING;
	req.Boolean = isRunning;

	mMainClient->SendRepBoolean(req);
}

void UCustomGameInstance::SendEquipping(bool isEquipping)
{
	GameObject::RepBoolean req;
	req.UserIdx = mTempPlayerInfo.UserIdx;
	req.Type = EQUIPPED;
	req.Boolean = isEquipping;

	mMainClient->SendRepBoolean(req);
}

void UCustomGameInstance::SendEquipmentChange(EItemType type, uint64 infoIndex)
{
	GameObject::Equipment req;
	req.UserIdx = mTempPlayerInfo.UserIdx;
	req.EquipType = (type == EItemType::Cloth) ? CLOTH : WEAPON;
	req.ItemIndex = infoIndex;

	mMainClient->SendEquipmentChange(req);
}

void UCustomGameInstance::SendEnemyRemoval(uint64 resIdx)
{
	GameObject::ResourceChange req;
	req.MapIdx = mTempPlayerInfo.CurrentMap;
	req.ResType = ENEMY;
	req.ResChangeType = REMOVE;
	req.ResIdx = resIdx;

	mMainClient->SendMapResourceChange(req);
}

void UCustomGameInstance::SendItemRemoval(uint64 resIdx)
{
	GameObject::ResourceChange req;
	req.MapIdx = mTempPlayerInfo.CurrentMap;
	req.ResType = ITEM;
	req.ResChangeType = REMOVE;
	req.ResIdx = resIdx;

	mMainClient->SendMapResourceChange(req);
}

void UCustomGameInstance::SendMapTransition(uint32 mapBefore, uint32 mapAfter)
{
	GameObject::MapTransition req;
	req.UserIdx = mTempPlayerInfo.UserIdx;
	req.MapIdx_Before = mapBefore;
	req.MapIdx_After = mapAfter;

	mMainClient->SendMapTransition(req);
}

void UCustomGameInstance::ReplicateLocation(uint64 userIdx, FVector loc, FRotator rot)
{
	// 맵 상의 리플리케이션 캐릭터에 위치,회전 정보를 적용
	auto toChange = mReplicatedCharacterList[userIdx];
	check(toChange);

	toChange->SetActorLocationAndRotation(loc, rot);
}

void UCustomGameInstance::ReplicateJumping(uint64 userIdx, bool isJumping)
{
	// 맵 상의 리플리케이션 캐릭터에 정보를 적용
	auto toChange = mReplicatedCharacterList[userIdx];
	check(toChange);

	toChange->SetJumping(isJumping);
}

void UCustomGameInstance::ReplicateRunning(uint64 userIdx, bool isRunning)
{
	// 맵 상의 리플리케이션 캐릭터에 정보를 적용
	auto toChange = mReplicatedCharacterList[userIdx];
	check(toChange);

	toChange->SetJumping(isRunning);
}

void UCustomGameInstance::ReplicateEquipping(uint64 userIdx, bool isEquipping)
{
	// 맵 상의 리플리케이션 캐릭터에 정보를 적용
	auto toChange = mReplicatedCharacterList[userIdx];
	check(toChange);

	toChange->SetJumping(isEquipping);
}

void UCustomGameInstance::ReplicateEquipmentChange(uint64 userIdx, EItemType type, uint64 infoIndex)
{
	// 맵 상의 리플리케이션 캐릭터에 정보를 적용
	auto toChange = mReplicatedCharacterList[userIdx];
	check(toChange);

	toChange->ChangeWeapon(infoIndex);
}

void UCustomGameInstance::ReplicateEnemyRemoval(uint32 mapIdx, uint64 resIdx)
{
	// 맵 상의 몹 캐릭터를 제거
	auto toRemove = mEnemyCharacterList[resIdx];
	check(toRemove);
	
	toRemove->Destroy();
}

void UCustomGameInstance::ReplicateItemRemoval(uint32 mapIdx, uint64 resIdx)
{
	// 맵 상의 아이템을 제거
	auto toRemove = mRootItemList[resIdx];
	check(toRemove);

	toRemove->Destroy();
}

void UCustomGameInstance::ReplicateMapTransition(uint64 userIdx, uint32 mapBefore, uint32 mapAfter)
{
	// 현재 맵에서 userIdx 플레이어가 사라짐
	if (mTempPlayerInfo.CurrentMap == mapBefore)
	{
		auto toRemove = mReplicatedCharacterList[userIdx];
		check(toRemove);

		toRemove->Destroy();
	}
	// 현재 맵에 userIdx 플레이어가 진입함
	else if (mTempPlayerInfo.CurrentMap == mapAfter)
	{
		const FVector spawnLocation(0.f, 0.f, 100.f); // 랜덤 스폰 위치 (곧 이어서 위치 정보가 전송되므로, 임시 처리)

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = GetWorld()->GetLevelScriptActor();
		spawnParam.Instigator = GetWorld()->GetLevelScriptActor()->GetInstigator();

		// 리플리케이션 캐릭터 스폰
		TObjectPtr<AReplicatedCharacter> spawnedCharacter = GetWorld()->SpawnActor<AReplicatedCharacter>(AReplicatedCharacter::StaticClass(), spawnLocation, FRotator(), spawnParam);
		if (spawnedCharacter)
		{
			mReplicatedCharacterList.Add(userIdx, spawnedCharacter);	// 맵에 추가
		}
	}
}
