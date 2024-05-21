#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ChattingServer.h"
#include <iostream>
#include <google/protobuf/port_def.inc>
#include <assert.h>

#define PACKET_SIZE 1024

unsigned int __stdcall HandleClient(void* data);

using namespace std;

ChattingServer* ChattingServer::mSingleton = nullptr;

ChattingServer::ChattingServer()
{
	assert(mSingleton == nullptr);

	mSingleton = this;
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
	mListenThread = thread(&ChattingServer::AcceptClient, this);

	char msg[PACKET_SIZE] = { 0 };
	while (true)
	{
		ZeroMemory(&msg, PACKET_SIZE);
		cin >> msg;
		if ((string)msg == "list")
		{
			cout << "Connected Client List" << endl;
			for (ChattingServerClient c : mClients)
			{
				cout << inet_ntoa(c.Address.sin_addr) << " --- " << ntohs(c.Address.sin_port) << " --- " << c.PlayerName << endl;
			}
		}
		else
		{
			for (ChattingServerClient c : mClients)
			{
				send(c.Socket, msg, strlen(msg), 0);
			}
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
	while (true)
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

		ChattingServerClient* client = new ChattingServerClient;
		client->Socket = client_socket;
		client->Address = clientAddr;
		client->PlayerName = "Anonymous";

		mhThreads.push_back((HANDLE)_beginthreadex(NULL, 0, HandleClient, client, 0, NULL));
		mClients.push_back(*client);
	}
}

unsigned int __stdcall HandleClient(void* data)
{
	ChattingServerClient* clientInfo = static_cast<ChattingServerClient*>(data);
	SOCKET socket_client = clientInfo->Socket;
	SOCKADDR_IN addr_client = clientInfo->Address;
	string playerName = clientInfo->PlayerName;

	delete clientInfo;

	auto server = ChattingServer::mSingleton;
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

		if (recvBuffer[0] == '#')
		{
			int cnt = 1;
			playerName = &recvBuffer[1];

			cout << playerName << endl;
		}
		else
		{
			// send to all client		
			// append player's name in front of message
			string sendString;
			sendString = playerName + " : ";
			sendString += recvBuffer;

			cout << sendString << endl;

			for (ChattingServerClient c : server->mClients)
			{

				send(c.Socket, sendString.c_str(), sendString.length(), 0); // 보낸 클라이언트를 포함해 모든 클라이언트에 메시지 전달
			}
		}
	} while (recvSize > 0);

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
	ChattingServer* server = new ChattingServer;

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