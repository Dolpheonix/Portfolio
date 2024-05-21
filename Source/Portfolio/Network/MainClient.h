#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>
#include "../Core/PlayerSaveObject.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class UCustomGameInstance;

class MainClient
{
public:
	MainClient();

public:
	int Init(TObjectPtr<UCustomGameInstance> gi);
	void Run();

	int SendLoginInfo(const char* id, const char* pw, bool isRegister);
	int SendNickname(const char* nickname);
	int HandleLoginResult(const char* result, bool isRegister);
	int HandleSubmitResult(const char* result, bool isSucceeded);

	int SaveGame(PlayerInfo toSave);

	void SetRunning(bool newVal);

public:
	static MainClient* mSingleton;

	TObjectPtr<UCustomGameInstance> mGameInstance;

	SOCKET mSocket;
	SOCKADDR_IN mAddress;
	thread mReceiveThread;

	bool bRunning;
};