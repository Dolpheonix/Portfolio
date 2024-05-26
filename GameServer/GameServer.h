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

/*
	GameServer	: Ŭ���̾�Ʈ, DB�� �α��� ����, �÷��� ������ ���� �ְ� �޴� ������ �մϴ�.
	- Protobuf	: ����/Ŭ���̾�Ʈ �� ���� ����ü�� ����ϱ� ���� Protobuf ����� ����߽��ϴ�.
	- DB		: MYSQL�� ����߽��ϴ�.
*/

using namespace std;

// ������ Ŭ���̾�Ʈ ����
struct GameServerClient
{
	SOCKET Socket;			// Ŭ���̾�Ʈ ����
	SOCKADDR_IN Address;	// Ŭ���̾�Ʈ�� �ּ�
	int userIdx;			// DB�� ����� ������ ���� �ε���. �α��� �Ŀ� ��ȯ�ǰ�, ���̺� �ÿ� ���.
};

class GameServer
{
public:
	GameServer();

public:
	int InitServerSocket();	// ������ ������ �ʱ�ȭ
	int BindToAddress();	// ���Ͽ� �ּҰ��� ���ε�
	int ListenToClient();	
	void Run();				// ���� ��ɾ� ó�� 

	void AcceptClient();	// Ŭ���̾�Ʈ Accept ó��
	void HandleClient(GameServerClient* client);	// Ŭ���̾�Ʈ ��/���� ó��

	// DB Method
	static void MakePlayerInfoFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeInventoryFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeQuestStatusFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeDefaultPlayerInfo(SaveObject::PlayerInfo& outPlayerInfo);	// ���̺� ������ ���� �� db�� ����
	void SavePlayerInfo(const SaveObject::PlayerInfo& playerInfo, int userIdx);
	bool SetNickname(const char* nickname, int userIdx);

public:
	static GameServer* mSingleton;		// �̱��� ��ü
		
	vector<GameServerClient> mClients;	// Ŭ���̾�Ʈ ���
	vector<HANDLE> mhThreads;			// Ŭ���̾�Ʈ�� ��/���� ���� ������
	vector<thread> mThreads;

	SOCKET mServerSocket;				// ���� ����
	SOCKADDR_IN mServerAddress;			// ���� �ּ�
	thread mListenThread;				// ���� ������

	// DB
	sql::mysql::MySQL_Driver* mSQLDriver;
	unique_ptr<sql::Connection> mSQLConnection;
};