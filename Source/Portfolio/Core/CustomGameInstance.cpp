#include "CustomGameInstance.h"
#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/PlayerCharacter.h"
#include "../Character/NpcCharacter.h"
#include "../Base/Func.h"
#include "../Base/LoadHelper.h"
#include "../Network/ChattingClient.h"
#include "CustomController.h"

UCustomGameInstance::UCustomGameInstance() : UGameInstance(), bLoaded(false), bIntro(false)
{
	mSavedChattings.SetNum(MAX_CHATTING_TO_SHOW);
}

void UCustomGameInstance::Init()
{
	if (bLoaded == false)
	{
		// 맵 정보 로딩
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UCustomGameInstance::OnMapLoaded);
		LoadGameData();	// 게임 데이터 로드
		
		mChattingClient = new ChattingClient;
		check(mChattingClient);

		if (mChattingClient->Init(this) == SOCKET_ERROR)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Can't init Chatting Client"));
		}

		mChattingThread = thread(&ChattingClient::Run, mChattingClient);

		bLoaded = true;
	}

	bIntro = true;

#if WITH_SERVER_CODE
	UE_LOG(LogTemp, Log, TEXT("DEDICATED SERVER!!"));
#endif

	UGameInstance::Init();
}

void UCustomGameInstance::Shutdown()
{
	mChattingClient->SetRunning(false);
	mChattingThread.join();

	UGameInstance::Shutdown();
}

void UCustomGameInstance::OpenIntro()
{
	bIntro = true;
	UGameplayStatics::OpenLevel(this, TEXT("Test_Intro"));
}

void UCustomGameInstance::LoadGameData()
{
	// 각 게임 데이터 로딩
	ItemInfo::LoadInfoListFromJson(mItemInfoList);
	FQuest::LoadInfoListFromJson(mQuestInfoList);
	FEnemyLabelInfo::LoadInfoListFromJson(mEnemyLabelInfoList);
	FEnemyInfo::LoadInfoListFromJson(mEnemyInfoList);

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

	LoadNpcList(world);
	LoadEnemyList(world);
}

void UCustomGameInstance::LoadNpcList(UWorld* world)
{
	const FString& currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
	const FString levelDataPath = FPaths::ProjectContentDir() + "Data/LevelData/" + currentLevelName + "/";

	TArray<TObjectPtr<AActor>> npcList;
	UGameplayStatics::GetAllActorsOfClass(this, ANpcCharacter::StaticClass(), npcList);
	const FString npcDataPath = levelDataPath + "Npc.json"; // Content/Data/Level/Npc.json

	FString result;
	if (FFileHelper::LoadFileToString(result, *npcDataPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("NpcData not loaded"));
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	const TArray<TSharedPtr<FJsonValue>>& npcInfoList = root->GetArrayField(TEXT("NpcList"));
	for (auto& npcInfo : npcInfoList)
	{
		const TSharedPtr<FJsonObject>& npcObject = npcInfo->AsObject();
		const int npcIndex = npcObject->GetIntegerField("Index");
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
		npc->LoadFromJson(npcObject);
	}
}

void UCustomGameInstance::LoadEnemyList(UWorld* world)
{
	const FString& currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
	const FString levelDataPath = FPaths::ProjectContentDir() + "Data/LevelData/" + currentLevelName + "/";

	const FString enemyDataPath = levelDataPath + "Enemy.json"; // Content/Data/Level/Enemy.json

	FString result;
	if (FFileHelper::LoadFileToString(result, *enemyDataPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("EnemyData not loaded"));
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	const TArray<TSharedPtr<FJsonValue>>& enemyValueList = root->GetArrayField(TEXT("EnemyList"));
	for (auto& enemyValue : enemyValueList)
	{
		const TSharedPtr<FJsonObject>& enemyObject = enemyValue->AsObject();

		const FVector spawnLocation = Helper::JsonToVector(enemyObject->GetArrayField("SpawnLocation"));
		const FEnemyInfo& enemyInfo = GetEnemyInfo(enemyObject->GetIntegerField("InfoIndex"));

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = world->GetLevelScriptActor();
		spawnParam.Instigator = world->GetLevelScriptActor()->GetInstigator();

		TObjectPtr<AEnemyCharacter> spawnedEnemy = world->SpawnActor<AEnemyCharacter>(enemyInfo.EnemyClass, spawnLocation, FRotator(), spawnParam);
		if (spawnedEnemy)
		{
			spawnedEnemy->SpawnDefaultController();
		}
	}
}

int UCustomGameInstance::CreateSaveFile(FString playerName)
{
	// 세이브 파일명 리스트 불러오기
	USaveSlot* ls = Cast<USaveSlot>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0));

	if (!ls)
	{
		// 리스트 파일이 없다면 새로 만듦
		ls = Cast<USaveSlot>(UGameplayStatics::CreateSaveGameObject(USaveSlot::StaticClass()));
	}
	else if (ls->mSlotNameList.Num() == MAX_SAVE_SLOT)
	{
		// 슬롯이 꽉참
		return -1;
	}

	const int emptySlot = ls->mSlotNameList.Num();
	ls->mSlotNameList.Add(FName(playerName));

	// 리스트 업데이트
	UGameplayStatics::SaveGameToSlot(ls, "SaveSlot", 0);

	// 세이브 파일 새로 생성
	UPlayerSaveObject* si = Cast<UPlayerSaveObject>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveObject::StaticClass()));
	check(si);

	// 기본값 지정
	si->mPlayerName = playerName;
	si->mCurrentLevel = 0;
	si->mCurrentMap = 0;
	si->mCurrentLocation = FVector(0.f, 0.f, -100.f);
	si->mCurrentGold = 0;

	si->mQuestTable.SetNum(mQuestInfoList.Num());
	for (int i = 0; i < mQuestInfoList.Num(); ++i)
	{
		si->mQuestTable[i].Type = mQuestInfoList[i].Type;
		si->mQuestTable[i].CurrProgress = EQuestProgressType::Available;	//TODO : 퀘스트 등록 가능/불가능 조건
		si->mQuestTable[i].SubStatus.SetNum(mQuestInfoList[i].SubQuests.Num());
		for (int j = 0; j < mQuestInfoList[i].SubQuests.Num(); ++j)
		{
			si->mQuestTable[i].SubStatus[j].bStarted = false;
			si->mQuestTable[i].SubStatus[j].bCompleted = false;
			si->mQuestTable[i].SubStatus[j].Type = mQuestInfoList[i].SubQuests[j].Type;
			si->mQuestTable[i].SubStatus[j].CurrAmount = 0;
		}
	}

	si->mSlotIndex = emptySlot;

	// 빈 슬롯 인덱스에 파일 저장
	UGameplayStatics::SaveGameToSlot(si, playerName, emptySlot);

	return emptySlot;
}

void UCustomGameInstance::SaveGame(TObjectPtr<APlayerCharacter> player)
{
	check(player);

	USaveSlot* saveSlot = Cast<USaveSlot>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0));
	check(saveSlot);

	const int slotIndex = player->GetSlotIndex();

	check(slotIndex < saveSlot->mSlotNameList.Num());

	const FString playerName = saveSlot->mSlotNameList[slotIndex].ToString();

	UPlayerSaveObject* so = Cast<UPlayerSaveObject>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveObject::StaticClass()));

	so->mPlayerName = player->mDisplayName;
	so->mCurrentLevel = player->GetCurrentLevel();
	so->mCurrentGold = player->GetCurrentGold();
	so->mCurrentMap = player->GetCurrentMapIndex();
	so->mCurrentLocation = player->GetActorLocation();
	so->mInventory = player->GetInventory_cpy();
	so->mQuestTable = player->GetQuestTable_cpy();
	so->mSlotIndex = player->GetSlotIndex();

	UGameplayStatics::SaveGameToSlot(so, player->mDisplayName, slotIndex);
}

void UCustomGameInstance::LoadGame(int slotIndex)
{
	USaveSlot* saveSlot = Cast<USaveSlot>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0));
	check(saveSlot);

	check(slotIndex < saveSlot->mSlotNameList.Num());

	const FString playerName = saveSlot->mSlotNameList[slotIndex].ToString();

	UPlayerSaveObject* ls = Cast<UPlayerSaveObject>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveObject::StaticClass()));
	ls = Cast<UPlayerSaveObject>(UGameplayStatics::LoadGameFromSlot(playerName, slotIndex));

	check(ls);

	mTempPlayerInfo.PlayerName = ls->mPlayerName;
	mTempPlayerInfo.CurrentLevel = ls->mCurrentLevel;
	mTempPlayerInfo.CurrentMap = ls->mCurrentMap;
	mTempPlayerInfo.CurrentLocation = ls->mCurrentLocation;
	mTempPlayerInfo.CurrentGold = ls->mCurrentGold;
	mTempPlayerInfo.QuestTable = ls->mQuestTable;
	mTempPlayerInfo.Inventory = ls->mInventory;
	mTempPlayerInfo.SlotIndex = ls->mSlotIndex;

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->CloseIntro();

	bIntro = false;

	// Send to Server
	const char* msg = TCHAR_TO_ANSI(*ls->mPlayerName);
	mChattingClient->SetName(msg);

	UGameplayStatics::OpenLevel(this, FName(mLevelList[mTempPlayerInfo.CurrentMap]));
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

PlayerInfo UCustomGameInstance::GetSaveData(int slotIndex) const
{
	USaveSlot* saveSlot = Cast<USaveSlot>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0));
	check(saveSlot);

	if (slotIndex >= saveSlot->mSlotNameList.Num())
	{
		PlayerInfo res;
		res.CurrentGold = -1;

		return res;
	}

	const FString playerName = saveSlot->mSlotNameList[slotIndex].ToString();

	UPlayerSaveObject* ls = Cast<UPlayerSaveObject>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveObject::StaticClass()));
	ls = Cast<UPlayerSaveObject>(UGameplayStatics::LoadGameFromSlot(playerName, slotIndex));

	PlayerInfo res;

	res.PlayerName = ls->mPlayerName;
	res.CurrentLevel = ls->mCurrentLevel;
	res.CurrentMap = ls->mCurrentMap;
	res.CurrentLocation = ls->mCurrentLocation;
	res.CurrentGold = ls->mCurrentGold;
	res.Inventory = ls->mInventory;
	res.SlotIndex = ls->mSlotIndex;

	return res;
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
		mSavedChattings[i] = mSavedChattings[i - 1];
	}

	mSavedChattings[0] = chat;

	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->UpdateChatting();
}
