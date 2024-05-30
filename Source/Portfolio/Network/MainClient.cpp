#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "MainClient.h"
#include "../ThirdParty/ProtoObject/LoginObject.pb.h"
#include "../ThirdParty/ProtoObject/SaveObject.pb.h"
#include "Async/Async.h"
#include "../Core/CustomGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include <assert.h>

#define PACKET_SIZE 2048

MainClient* MainClient::mSingleton = nullptr;

MainClient::MainClient() : bRunning(false), mGameInstance(nullptr)
{
	assert(mSingleton == nullptr);

	mSingleton = this;
}

int MainClient::Init(TObjectPtr<UCustomGameInstance> gi)
{
	check(gi);

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	mAddress = {};
	mAddress.sin_family = AF_INET;
	mAddress.sin_port = htons(5555);
	mAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	mGameInstance = gi;

	return 0;
}

void MainClient::Run()
{
	bRunning = true;

	while (true)
	{
		if (!connect(mSocket, (SOCKADDR*)&mAddress, sizeof(mAddress)))
		{
			break;
		}
	}

	u_long nonblock = 1;
	ioctlsocket(mSocket, FIONBIO, &nonblock);	// 논블록 소켓으로 지정해, Running 상태를 항상 확인해 게임 종료 시에 스레드가 정상 종료됩니다.

	char recvBuffer[PACKET_SIZE] = {};

	while (bRunning == true)
	{
		ZeroMemory(&recvBuffer, PACKET_SIZE);
		int received = recv(mSocket, recvBuffer, PACKET_SIZE, 0);

		if (received == SOCKET_ERROR)
		{
			continue;
		}
		else
		{
			string recvString(recvBuffer, received);
			AsyncTask(ENamedThreads::GameThread, [this, recvString]()	// 게임 인스턴스에 접근하기 때문에, 언리얼 GameThread에서 실행되어야 합니다.
			{
				string header = recvString.substr(0, 2);
				string data = recvString.substr(2, string::npos);
				if (header == "l_")			// 로그인 결과
				{
					HandleLoginResult(data, false);
				}
				else if (header == "r_")	// 회원가입 결과
				{
					HandleLoginResult(data, true);
				}
				else if (header == "n_")	// 닉네임 설정 성공
				{
					HandleSubmitResult(data.c_str(), true);
				}
				else if (header == "f_")	// 닉네임 설정 실패
				{
					HandleSubmitResult(nullptr, false);
				}
				else
				{
					//TOOO : REPLICATION, ...
				}
			});
		}
	}

	closesocket(mSocket);
}

int MainClient::SendLoginInfo(const char* id, const char* pw, bool isRegister)
{
	LoginObject::LoginInfo sendInfo = {};
	sendInfo.set_id(id);
	sendInfo.set_password(pw);

	string sendStr = isRegister ? "r_" : "l_";

	string serialized = sendInfo.SerializeAsString();
	sendStr += serialized;
	return send(mSocket, sendStr.c_str(), sendStr.length(), 0);
}

int MainClient::SendNickname(const char* nickname)
{
	string sendStr = "n_";

	sendStr += nickname;
	return send(mSocket, sendStr.c_str(), sendStr.length(), 0);
}

int MainClient::HandleLoginResult(const string& result, bool isRegister)
{
	SaveObject::PlayerInfo recvInfo = {};

	if (!recvInfo.ParseFromString(result))
	{
		return -1;
	}

	PlayerInfo info;
	info.ConvertFromProto(recvInfo);	// Proto 구조체에서 Unreal 구조체로 변환
	mGameInstance->HandleLoginResult(info, isRegister);

	return 0;
}

int MainClient::HandleSubmitResult(const char* result, bool isSucceeded)
{
	mGameInstance->HandleNicknameResult(result, isSucceeded);

	return 0;
}

int MainClient::SaveGame(PlayerInfo toSave)
{
	SaveObject::PlayerInfo sendInfo = {};
	toSave.ConvertToProto(sendInfo);

	string sendStr = "s_";

	sendStr += sendInfo.SerializeAsString();
	return send(mSocket, sendStr.c_str(), sendStr.length(), 0);
}

void MainClient::SetRunning(bool newVal)
{
	bRunning = newVal;
}
