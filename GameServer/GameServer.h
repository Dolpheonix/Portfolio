#pragma once

#include <WinSock2.h>
#include <thread>
#include <string>
#include <vector>
#include <mysql.h>
#include <ProtoObject/SaveObject.pb.h>
#include <ProtoObject/LoginObject.pb.h>
#include <mysql/jdbc.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct GameServerClient
{
	SOCKET Socket;
	SOCKADDR_IN Address;

	int userIdx;
};

class GameServer
{
public:
	GameServer();

public:
	int InitServerSocket();
	int BindToAddress();
	int ListenToClient();
	void Run();

	void AcceptClient();

	// DB Method
	static void MakePlayerInfoFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeInventoryFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeQuestStatusFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeDefaultPlayerInfo(SaveObject::PlayerInfo& outPlayerInfo);
	void SavePlayerInfo(const SaveObject::PlayerInfo& playerInfo, int userIdx);
	bool SetNickname(const char* nickname, int userIdx);

public:
	static GameServer* mSingleton;

	vector<GameServerClient> mClients;
	vector<HANDLE> mhThreads;

	SOCKET mServerSocket;
	SOCKADDR_IN mServerAddress;
	thread mListenThread;

	// DB
	sql::mysql::MySQL_Driver* mSQLDriver;
	unique_ptr<sql::Connection> mSQLConnection;
};