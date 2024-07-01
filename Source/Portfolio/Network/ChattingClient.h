#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class UCustomGameInstance;

/*
	채팅 서버와 연결되어 채팅 데이터를 송/수신하는 클래스
	- winsock으로 구현됨
	- 서버-클라이언트 흐름
		1. 채팅 입력창에 채팅을 입력하면 채팅 서버에 데이터 전송
		2. 채팅 서버에서 클라이언트 캐릭터의 닉네임을 붙여 본인 포함 모든 클라이언트에 데이터 전송
		3. 각 클라이언트는 전송받은 채팅을 채팅창에 표시
*/

class ChattingClient
{
private:
	ChattingClient();
	ChattingClient(const ChattingClient& rhs) = delete;
	ChattingClient& operator=(const ChattingClient& rhs) = delete;
	~ChattingClient() {}

public:
	static ChattingClient* GetSingleton();
	
	// 소켓 생성 및 주소 설정
	int Init(TObjectPtr<UCustomGameInstance> gi);	
	// 서버와 연결 시도 및 데이터 수신
	void Run();	
	// 서버에 데이터 전송
	int SendChatting(const char* msg);	
	// 채팅에 붙여질 닉네임을 전송
	void SetName(FString str);	

	void SetRunning(bool newVal);

public:
	TObjectPtr<UCustomGameInstance> mGameInstance;	// 게임 인스턴스

	SOCKET mSocket;			// 클라이언트 소켓
	SOCKADDR_IN mAddress;	// 연결할 주소

	string mPlayerName;		// 채팅 닉네임

	bool bRunning;			// 게임이 현재 실행중인가?
};