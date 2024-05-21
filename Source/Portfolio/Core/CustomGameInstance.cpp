#include "CustomGameInstance.h"
#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/PlayerCharacter.h"
#include "../Character/NpcCharacter.h"
#include "../Base/Func.h"
#include "../Base/LoadHelper.h"
#include "../Network/ChattingClient.h"
#include "../Network/MainClient.h"
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
		
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Can't start WSA"));
		}

		// 게임 서버와 연결
		mMainClient = new MainClient;
		check(mMainClient);
		if (mMainClient->Init(this) == SOCKET_ERROR)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Can't init Main Client"));
		}

		mMainClientThread = thread(&MainClient::Run, mMainClient);

		// 채팅 서버와 연결
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
	mMainClient->SetRunning(false);
	mChattingClient->SetRunning(false);


	// Save before Shutdown
	if (bIntro == false)
	{
		mMainClient->SaveGame(mTempPlayerInfo);
	}

	mMainClientThread.join();
	mChattingThread.join();

	WSACleanup();

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

void UCustomGameInstance::SendLoginInfo(const char* id, const char* pw, bool isRegister)
{
	mMainClient->SendLoginInfo(id, pw, isRegister);
}

void UCustomGameInstance::SendNickname(const char* nickname)
{
	mMainClient->SendNickname(nickname);
}

void UCustomGameInstance::HandleLoginResult(PlayerInfo received, bool isRegister)
{
	if (received.CurrentMap == 10000)
	{
		TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
		check(controller);
		controller->NotifyLoginFailed(isRegister);
	}
	else if (isRegister == true)
	{
		mTempPlayerInfo = received;

		TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
		check(controller);
		controller->NotifyRegisterSucceeded();
	}
	else 
	{
		mTempPlayerInfo = received;
		StartGame();
	}
}

void UCustomGameInstance::HandleNicknameResult(const char* nickname, bool isSubmitted)
{
	if (isSubmitted == true)
	{
		mTempPlayerInfo.PlayerName = nickname;
		StartGame();
	}
	else
	{
		TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
		check(controller);
		controller->NotifyNicknameDuplicated();
	}
}

void UCustomGameInstance::StartGame()
{
	TObjectPtr<ACustomController> controller = Cast<ACustomController>(UGameplayStatics::GetPlayerController(this, 0));
	check(controller);
	controller->CloseIntro();

	bIntro = false;

	// Send to Server
	const char* msg = TCHAR_TO_ANSI(*mTempPlayerInfo.PlayerName);
	mChattingClient->SetName(msg);

	UGameplayStatics::OpenLevel(this, FName(mLevelList[mTempPlayerInfo.CurrentMap]));
}
