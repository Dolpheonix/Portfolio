#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>
#include "../Core/PlayerSaveObject.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class UCustomGameInstance;

/*
	GameServer와 연결되는 클라이언트 클래스
	- Protobuf 구조체로 서버와 데이터를 송/수신합니다.
*/

class MainClient
{
public:
	MainClient();

public:
	int Init(TObjectPtr<UCustomGameInstance> gi);	// 소켓 초기화 및 서버와 연결 시도
	void Run();

	int SendLoginInfo(const char* id, const char* pw, bool isRegister);	// 서버에 로그인 정보 전송
	int SendNickname(const char* nickname);								// 닉네임 설정 후 전송
	int HandleLoginResult(const string& result, bool isRegister);		// 로그인 결과를 핸들링
	int HandleSubmitResult(const char* result, bool isSucceeded);		// 닉네임 설정 결과를 핸들링

	int SaveGame(PlayerInfo toSave);	// 현재 게임 상태를 서버에 저장

	void SetRunning(bool newVal);

public:
	static MainClient* mSingleton;

	TObjectPtr<UCustomGameInstance> mGameInstance;	// 실행중인 게임 인스턴스

	SOCKET mSocket;
	SOCKADDR_IN mAddress;

	bool bRunning;
};