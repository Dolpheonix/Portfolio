#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class UCustomGameInstance;

class ChattingClient
{
public:
	ChattingClient();

public:
	int Init(TObjectPtr<UCustomGameInstance> gi);
	void Run();

	int SendChatting(const char* msg);

	void SetName(FString str);

	void SetRunning(bool newVal);

public:
	static ChattingClient* mSingleton;

	TObjectPtr<UCustomGameInstance> mGameInstance;

	SOCKET mSocket;
	SOCKADDR_IN mAddress;
	thread mReceiveThread;

	string mPlayerName;

	bool bRunning;
};