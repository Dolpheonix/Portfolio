#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>
#include "../Core/PlayerSaveObject.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class UCustomGameInstance;

/*
	GameServer�� ����Ǵ� Ŭ���̾�Ʈ Ŭ����
	- Protobuf ����ü�� ������ �����͸� ��/�����մϴ�.
*/

class MainClient
{
public:
	MainClient();

public:
	int Init(TObjectPtr<UCustomGameInstance> gi);	// ���� �ʱ�ȭ �� ������ ���� �õ�
	void Run();	// ���� ������ ����

	int SendLoginInfo(const char* id, const char* pw, bool isRegister);	// ������ �α��� ���� ����
	int SendNickname(const char* nickname);								// �г��� ���� �� ����
	int HandleLoginResult(const char* result, bool isRegister);			// �α��� ����� �ڵ鸵
	int HandleSubmitResult(const char* result, bool isSucceeded);		// �г��� ���� ����� �ڵ鸵

	int SaveGame(PlayerInfo toSave);	// ���� ���� ���¸� ������ ����

	void SetRunning(bool newVal);

public:
	static MainClient* mSingleton;

	TObjectPtr<UCustomGameInstance> mGameInstance;	// �������� ���� �ν��Ͻ�

	SOCKET mSocket;
	SOCKADDR_IN mAddress;

	bool bRunning;
};