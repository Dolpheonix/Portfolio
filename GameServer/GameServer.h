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
	GameServer	: 클라이언트, DB와 로그인 정보, 플레이 데이터 등을 주고 받는 역할을 합니다.
	- Protobuf	: 서버/클라이언트 간 동일 구조체를 사용하기 위해 Protobuf 방식을 사용했습니다.
	- DB		: MYSQL을 사용했습니다.
*/

using namespace std;

// 접속한 클라이언트 정보
struct GameServerClient
{
	SOCKET Socket;			// 클라이언트 소켓
	SOCKADDR_IN Address;	// 클라이언트의 주소
	int userIdx;			// DB에 저장된 유저의 고유 인덱스. 로그인 후에 반환되고, 세이브 시에 사용.
};

class GameServer
{
public:
	GameServer();

public:
	int InitServerSocket();	// 서버의 소켓을 초기화
	int BindToAddress();	// 소켓에 주소값을 바인딩
	int ListenToClient();	
	void Run();				// 서버 명령어 처리 

	void AcceptClient();	// 클라이언트 Accept 처리
	void HandleClient(GameServerClient* client);	// 클라이언트 송/수신 처리

	// DB Method
	static void MakePlayerInfoFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeInventoryFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeQuestStatusFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo);
	static void MakeDefaultPlayerInfo(SaveObject::PlayerInfo& outPlayerInfo);	// 세이브 데이터 생성 및 db에 저장
	void SavePlayerInfo(const SaveObject::PlayerInfo& playerInfo, int userIdx);
	bool SetNickname(const char* nickname, int userIdx);

public:
	static GameServer* mSingleton;		// 싱글톤 객체
		
	vector<GameServerClient> mClients;	// 클라이언트 목록
	vector<HANDLE> mhThreads;			// 클라이언트별 송/수신 관리 스레드
	vector<thread> mThreads;

	SOCKET mServerSocket;				// 서버 소켓
	SOCKADDR_IN mServerAddress;			// 서버 주소
	thread mListenThread;				// 리슨 스레드

	// DB
	sql::mysql::MySQL_Driver* mSQLDriver;
	unique_ptr<sql::Connection> mSQLConnection;
};