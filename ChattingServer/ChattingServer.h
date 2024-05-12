#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct ChattingServerClient
{
	SOCKET Socket;
	SOCKADDR_IN Address;

	string PlayerName;
};

class ChattingServer
{
public:
	ChattingServer();

public:
	int InitServerSocket();
	int BindToAddress();
	int ListenToClient();
	void Run();

	void AcceptClient();

public:
	static ChattingServer* mSingleton;

	vector<ChattingServerClient> mClients;
	vector<HANDLE> mhThreads;

	SOCKET mServerSocket;
	SOCKADDR_IN mServerAddress;
	thread mListenThread;
};