#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ChattingClient.h"
#include "Async/Async.h"
#include "../Core/CustomGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include <assert.h>

#define PACKET_SIZE 1024

ChattingClient::ChattingClient() : bRunning(false), mGameInstance(nullptr)
{
}

ChattingClient* ChattingClient::GetSingleton()
{
	static ChattingClient cc;
	return &cc;
}

int ChattingClient::Init(TObjectPtr<UCustomGameInstance> gi)
{
	check(gi);

	// 소켓 생성
	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	// 주소 설정
	mAddress = {};
	mAddress.sin_family = AF_INET;
	mAddress.sin_port = htons(4444);
	mAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	mGameInstance = gi;

	return 0;
}

void ChattingClient::Run()
{
	bRunning = true;

	while (bRunning == true)
	{
		// 서버와 연결
		if (!connect(mSocket, (SOCKADDR*)&mAddress, sizeof(mAddress)))
		{
			break;
		}
	}

	// 논블록 소켓으로 변경 (bRunning 상태를 계속 체크해야 함)
	u_long nonblock = 1;
	ioctlsocket(mSocket, FIONBIO, &nonblock);

	char buffer[PACKET_SIZE] = {};

	while (bRunning == true)
	{
		ZeroMemory(&buffer, PACKET_SIZE);
		int received = recv(mSocket, buffer, PACKET_SIZE, 0);
		
		if (received == SOCKET_ERROR)
		{
			continue;
		}
		else
		{
			// 인게임 오브젝트는 GameThread 내에서만
			AsyncTask(ENamedThreads::GameThread, [this, buffer]() 
			{
				mGameInstance->HandleChatMessage(buffer);
			});
		}
	}

	closesocket(mSocket);
}

int ChattingClient::SendChatting(const char* msg)
{
	return send(mSocket, msg, strlen(msg), 0);
}

void ChattingClient::SetName(FString str)
{
	string nameMsg = "#";	// 닉네임 설정은 #을 헤더로 붙임
	nameMsg += TCHAR_TO_ANSI(*str);

	send(mSocket, nameMsg.c_str(), nameMsg.length(), 0);
}

void ChattingClient::SetRunning(bool newVal)
{
	bRunning = newVal;
}
