#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ChattingServer.h"
#include <iostream>
#include <assert.h>

#define PACKET_SIZE 1024

using namespace std;

ChattingServer::ChattingServer() : bRunning(false)
{
}

ChattingServer* ChattingServer::GetSingleton()
{
	static ChattingServer cs;
	return &cs;
}

int ChattingServer::InitServerSocket()
{
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mServerSocket == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	return 0;
}

int ChattingServer::BindToAddress()
{
	mServerAddress = {};
	mServerAddress.sin_family = AF_INET;
	mServerAddress.sin_port = htons(4444);
	mServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	return ::bind(mServerSocket, (SOCKADDR*)&mServerAddress, sizeof(mServerAddress));
}

int ChattingServer::ListenToClient()
{
	return listen(mServerSocket, SOMAXCONN);
}

void ChattingServer::Run()
{
	bRunning = true;

	mListenThread = thread(&ChattingServer::AcceptClient, this);

	cout << "Chatting Server Started" << endl;

	char msg[PACKET_SIZE] = {};
	while (true)
	{
		ZeroMemory(&msg, PACKET_SIZE);
		cin >> msg;
		if ((string)msg == "list")	// 현재 연결된 클라이언트를 콘솔창에 출력
		{
			cout << "Connected Client List" << endl;
			for (ChattingServerClient c : mClients)
			{
				cout << inet_ntoa(c.Address.sin_addr) << " --- " << ntohs(c.Address.sin_port) << " --- " << c.PlayerName << endl;
			}
		}
		else if ((string)msg == "quit")
		{
			bRunning = false;
			break;
		}
		else
		{
			// TODO : 그 외 명령어
		}
	}

	mListenThread.join();
	for (HANDLE h : mhThreads)
	{
		WaitForSingleObject(h, INFINITE);
		CloseHandle(h);
	}

	closesocket(mServerSocket);
}

void ChattingServer::AcceptClient()
{
	while (bRunning == true)
	{
		SOCKADDR_IN clientAddr = {};
		int client_size = sizeof(clientAddr);

		SOCKET client_socket = accept(mServerSocket, (SOCKADDR*)&clientAddr, &client_size);
		if (client_socket == INVALID_SOCKET)
		{
			cerr << "Client Connection Failed : " << WSAGetLastError() << endl;
			closesocket(client_socket);
		}

		if (!WSAGetLastError())
		{
			cout << "Client Connected --- IP : " << inet_ntoa(clientAddr.sin_addr) << " --- Port : " << ntohs(clientAddr.sin_port) << endl;
		}
		else continue;

		// 연결된 클라이언트마다 구조체를 생성해 정보를 보관
		ChattingServerClient* client = new ChattingServerClient;
		client->Socket = client_socket;
		client->Address = clientAddr;
		client->PlayerName = "Anonymous";

		// 각 클라이언트 별로 송수신 스레드를 실행
		mhThreads.push_back((HANDLE)_beginthreadex(NULL, 0, &ChattingServer::HandleClient, client, 0, NULL));
		mClients.push_back(*client);
	}
}

unsigned int __stdcall ChattingServer::HandleClient(void* data)
{
	ChattingServerClient* clientInfo = static_cast<ChattingServerClient*>(data);
	SOCKET socket_client = clientInfo->Socket;
	SOCKADDR_IN addr_client = clientInfo->Address;
	string playerName = clientInfo->PlayerName;

	delete clientInfo;

	auto server = GetSingleton();
	char recvBuffer[1024];
	int recvSize;

	do
	{
		ZeroMemory(&recvBuffer, 1024);
		recvSize = recv(socket_client, recvBuffer, sizeof(recvBuffer), 0);

		if (recvSize <= 0)
		{
			continue;
		}

		if (recvBuffer[0] == '#')	// 닉네임 설정
		{
			int cnt = 1;
			playerName = &recvBuffer[1];

			cout << playerName << endl;
		}
		else
		{
			// 채팅 메시지가 전송됨 --> 모든 클라이언트에 해당 메시지 전송		
			// 메시지 앞에 클라이언트의 닉네임을 붙여서 전송
			string sendString;
			sendString = playerName + " : ";
			sendString += recvBuffer;

			cout << sendString << endl;

			for (ChattingServerClient c : server->mClients)
			{

				send(c.Socket, sendString.c_str(), sendString.length(), 0);
			}
		}
	} while ((recvSize > 0) && (server->bRunning == true));

	// 연결이 끊겼다면, 클라이언트 목록에서 삭제한다.
	for (size_t i = 0; i < server->mClients.size(); ++i)
	{
		if (server->mClients[i].Socket == socket_client)
		{
			cout << "Client Disconnected : " << inet_ntoa(addr_client.sin_addr) << "(port " << ntohs(addr_client.sin_port) << ") : " << endl;
			server->mClients.erase(server->mClients.begin() + i);
			break;
		}
	}

	closesocket(socket_client);
	_endthreadex(0);

	return 0;
}

int main(int argc, char argv[])
{
	// Init Server Class
	ChattingServer* server = ChattingServer::GetSingleton();

	// Init WSA
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "WSAStartup failed" << endl;
		return 1;
	}

	if (server->InitServerSocket() == SOCKET_ERROR)
	{
		cout << "Server Socket Initialization Failed" << endl;
		return 1;
	}

	if (server->BindToAddress() == SOCKET_ERROR)
	{
		cout << "Server Socket Bind Failed" << endl;
		closesocket(server->mServerSocket);
		WSACleanup();
		return 1;
	}

	if (server->ListenToClient() == SOCKET_ERROR)
	{
		cout << "Server Socket Listen Failed" << endl;
		closesocket(server->mServerSocket);
		WSACleanup();
		return 1;
	}

	server->Run();

	WSACleanup();

	return 0;
}