#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// 접속한 클라이언트 정보
struct ChattingServerClient
{
	SOCKET Socket;
	SOCKADDR_IN Address;

	string PlayerName;
};

class ChattingServer
{
private:
	ChattingServer();
	ChattingServer(const ChattingServer& rhs) = delete;
	ChattingServer& operator=(const ChattingServer& rhs) = delete;
	~ChattingServer() {};

public:
	// 인스턴스 반환
	static ChattingServer* GetSingleton();
	// 소켓 생성
	int InitServerSocket();
	// 주소 설정
	int BindToAddress();
	// 클라이언트 접속 대기
	int ListenToClient();
	// 서버 명령어 처리
	void Run();
	// 클라이언트 Accept 처리
	void AcceptClient();
	// 각 클라이언트 접속 핸들러
	static unsigned int __stdcall HandleClient(void* data);

public:
	// 접속한 클라이언트 리스트
	vector<ChattingServerClient> mClients;
	// 클라이언트별 스레드 핸들
	vector<HANDLE> mhThreads;
	// 서버 소켓
	SOCKET mServerSocket;
	// 서버 주소
	SOCKADDR_IN mServerAddress;
	// Accept 처리용 스레드
	thread mListenThread;

	bool bRunning;
};