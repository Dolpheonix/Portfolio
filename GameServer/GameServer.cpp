#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "GameServer.h"
#include <iostream>
#include <fstream>
#include <json.h>

#define PACKET_SIZE 1024

GameServer* GameServer::mSingleton = nullptr;

GameServer::GameServer()
{
	assert(mSingleton == nullptr);

	mSQLDriver = nullptr;
	mSQLConnection = nullptr;
	mSingleton = this;
}

int GameServer::InitServerSocket()
{
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mServerSocket == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	// DB�� ����
	mSQLDriver = sql::mysql::get_mysql_driver_instance();
	mSQLConnection.reset(mSQLDriver->connect("tcp://127.0.0.1:3306", "root", "redapple67#"));
	mSQLConnection->setSchema("portfolio");

	return 0;
}

int GameServer::BindToAddress()
{
	mServerAddress = {};
	mServerAddress.sin_family = AF_INET;
	mServerAddress.sin_port = htons(5555);	// 5555�� ��Ʈ ���
	mServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	return ::bind(mServerSocket, (SOCKADDR*)&mServerAddress, sizeof(mServerAddress));
}

int GameServer::ListenToClient()
{
	return listen(mServerSocket, SOMAXCONN);
}

void GameServer::Run()
{
	mListenThread = thread(&GameServer::AcceptClient, this);	// Accept ������ ������ ����

	// ���� ��ɾ� ó��
	char msg[PACKET_SIZE] = {0};
	while (true)
	{
		ZeroMemory(&msg, PACKET_SIZE);
		cin >> msg;
		if ((string)msg == "list")	// ����� Ŭ���̾�Ʈ ��� ���
		{
			cout << "Connected Client List" << endl;
			for (GameServerClient c : mClients)
			{
				cout << inet_ntoa(c.Address.sin_addr) << " --- " << ntohs(c.Address.sin_port) << endl;
			}
		}
	}

	// ������ ���� �� ���� ����
	mListenThread.join();
	for (thread& t : mThreads)
	{
		t.join();
	}

	closesocket(mServerSocket);
}

void GameServer::AcceptClient()
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

		GameServerClient* client = new GameServerClient;
		client->Socket = client_socket;
		client->Address = clientAddr;

		mThreads.push_back(thread(&GameServer::HandleClient, this, client));
		mClients.push_back(*client);
	}
}

void GameServer::HandleClient(GameServerClient* clientInfo)
{
	SOCKET socket_client = clientInfo->Socket;
	SOCKADDR_IN addr_client = clientInfo->Address;

	char recvBuffer[PACKET_SIZE];
	int recvSize;

	unique_ptr<sql::PreparedStatement> pstmt = nullptr;
	unique_ptr<sql::ResultSet> sqlres = nullptr;

	do
	{
		ZeroMemory(&recvBuffer, PACKET_SIZE);
		recvSize = recv(socket_client, recvBuffer, sizeof(recvBuffer), 0);

		string recvStr = recvBuffer;
		string header = recvStr.substr(0, 2);

		if (recvSize <= 0)
		{
			continue;
		}

		if (header == "r_")	// ���� �õ�
		{
			LoginObject::LoginInfo li;

			if (!li.ParseFromString(&recvBuffer[2]))
			{
				//TODO : Ŭ���̾�Ʈ�� ������ �Է� �˸�
				continue;
			}

			const string id = li.id();
			const string pw = li.password();

			cout << "Register asked from Client " << inet_ntoa(addr_client.sin_addr) << endl;

			// ȸ������ SP ����
			pstmt.reset(mSQLConnection->prepareStatement("CALL TryRegister(?, ?)"));
			pstmt->setString(0, id);
			pstmt->setString(1, pw);

			sqlres.reset(pstmt->executeQuery());

			SaveObject::PlayerInfo sendInfo = {};

			if (sqlres->next())
			{
				int userIdx = sqlres->getInt("userIdx");
				if (userIdx < 0)	// ȸ������ ����
				{
					sendInfo.set_map(10000);	// Ŭ���̾�Ʈ�� ���и� �˸��� �÷��� ����
					// ����ȭ �� ����
					const string sendStr = "r_" + sendInfo.SerializeAsString();	
					send(socket_client, sendStr.c_str(), sendStr.length(), 0);

					continue;
				}

				clientInfo->userIdx = userIdx;
			}

			// �⺻ ���̺� ������ ����
			GameServer::MakeDefaultPlayerInfo(sendInfo);
			// DB�� ������ ������ ����
			SavePlayerInfo(sendInfo, clientInfo->userIdx);

			// ����ȭ �� ����
			const string sendStr = "r_" + sendInfo.SerializeAsString();
			send(socket_client, sendStr.c_str(), sendStr.length(), 0);
		}
		else if (header == "n_") // ȸ������ ��, �г��� ����
		{
			const string nickname = &recvBuffer[2];

			if (SetNickname(nickname.c_str(), clientInfo->userIdx))
			{
				// �г��� ������ ������. ������ �г����� Ŭ���̾�Ʈ�� �ٽ� ����
				const string sendStr = "n_" + nickname;
				send(socket_client, sendStr.c_str(), sendStr.length(), 0);
			}
			else
			{
				// �г��� ������ ���������� Ŭ���̾�Ʈ�� �˸�
				const string sendStr = "f_FAILED";
				send(socket_client, sendStr.c_str(), sendStr.length(), 0);
			}
		}
		else if (header == "l_")	// �α��� �õ�
		{
			LoginObject::LoginInfo li;

			if (!li.ParseFromString(&recvBuffer[2]))
			{
				//TODO : Ŭ���̾�Ʈ�� ������ �Է� �˸�
				continue;
			}

			const string id = li.id();
			const string pw = li.password();

			cout << "Login asked from Client " << inet_ntoa(addr_client.sin_addr) << endl;

			// �α��� SP ����
			pstmt.reset(mSQLConnection->prepareStatement("CALL TryLogin(?, ?)"));
			pstmt->setString(0, id);
			pstmt->setString(1, pw);

			sqlres.reset(pstmt->executeQuery());

			SaveObject::PlayerInfo sendInfo = {};

			if (sqlres->next())
			{
				int userIdx = sqlres->getInt("userIdx");
				if (userIdx < 0)
				{
					// �α��� ����
					cout << "Client " << inet_ntoa(addr_client.sin_addr) << " : Login Failed" << endl;
					sendInfo.set_map(10000);
				}
				else
				{
					cout << "Client " << inet_ntoa(addr_client.sin_addr) << "'s userIdx : " << userIdx << endl;
					clientInfo->userIdx = userIdx;

					// userIdx�� ���� �� ���̺��� ���� �����͸� fetch�ؿ�
					if (pstmt->getMoreResults())
					{
						//from TABLE PlayerInfo
						sqlres.reset(pstmt->getResultSet());

						GameServer::MakePlayerInfoFromResultSet(sqlres.get(), sendInfo);
					}

					if (pstmt->getMoreResults())
					{
						//from TABLE Inventory
						sqlres.reset(pstmt->getResultSet());

						SaveObject::Inventory inventory;
						GameServer::MakeInventoryFromResultSet(sqlres.get(), sendInfo);
					}

					if (pstmt->getMoreResults())
					{
						//from TABLE QuestStatus
						sqlres.reset(pstmt->getResultSet());

						GameServer::MakeQuestStatusFromResultSet(sqlres.get(), sendInfo);
					}
				}
			}

			// ����ȭ �� ����
			const string sendStr = "l_" + sendInfo.SerializeAsString();
			send(socket_client, sendStr.c_str(), sendStr.length(), 0);
		}
		else if (header == "s_")	// ���� ���� ����
		{
			SaveObject::PlayerInfo pi;

			if (!pi.ParseFromString(&recvBuffer[2]))
			{
				//TODO : Ŭ���̾�Ʈ�� ������ �Է� �˸�
				continue;
			}

			SavePlayerInfo(pi, clientInfo->userIdx);
		}
	} while (recvSize > 0);

	for (size_t i = 0; i < mClients.size(); ++i)
	{
		if (mClients[i].Socket == socket_client)
		{
			cout << "Client Disconnected : " << inet_ntoa(addr_client.sin_addr) << "(port " << ntohs(addr_client.sin_port) << ") : " << endl;
			mClients.erase(mClients.begin() + i);
			break;
		}
	}

	closesocket(socket_client);
}

void GameServer::MakePlayerInfoFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo)
{
	outPlayerInfo.set_name(set->getString("nickname"));
	outPlayerInfo.set_level(set->getUInt("playerLevel"));
	outPlayerInfo.set_map(set->getUInt("mapIdx"));
	outPlayerInfo.set_loc_x(set->getDouble("loc_x"));
	outPlayerInfo.set_loc_y(set->getDouble("loc_y"));
	outPlayerInfo.set_loc_z(set->getDouble("loc_z"));
	outPlayerInfo.set_gold(set->getUInt("gold"));
}

void GameServer::MakeInventoryFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo)
{
	SaveObject::Inventory* outInventory = outPlayerInfo.mutable_inventory();

	for (int i = 0; i < SaveObject::Inventory_ItemType_ItemType_ARRAYSIZE; ++i)
	{
		outInventory->add_typeinventory();
	}

	while (set->next())
	{
		string typeStr = set->getString("itemType");
		
		SaveObject::GameItem* item = nullptr;
		
		if (typeStr == "CLOTH")
		{
			item = outInventory->mutable_typeinventory(0)->add_items();
		}
		else if(typeStr == "WEAPON")
		{
			item = outInventory->mutable_typeinventory(1)->add_items();
		}
		else if (typeStr == "ITEM")
		{
			item = outInventory->mutable_typeinventory(2)->add_items();
		}
		else
		{
			return;
		}

		item->set_index(set->getUInt64("infoIndex"));
		item->set_num(set->getUInt64("num"));
	}
}

void GameServer::MakeQuestStatusFromResultSet(sql::ResultSet* set, SaveObject::PlayerInfo& outPlayerInfo)
{
	while (set->next())
	{
		if (set->getUInt("questIdx") < outPlayerInfo.queststatus_size())
		{
			auto qs = outPlayerInfo.add_queststatus();
			qs->set_index(set->getUInt("questIdx"));

			string progressStr = set->getString("progress");
			if (progressStr == "Unavailable")
			{
				qs->set_progresstype(SaveObject::QuestStatus_QuestProgressType_UNAVAILABLE);
			}
			else if (progressStr == "Available")
			{
				qs->set_progresstype(SaveObject::QuestStatus_QuestProgressType_AVAILABLE);
			}
			else if (progressStr == "InProgress")
			{
				qs->set_progresstype(SaveObject::QuestStatus_QuestProgressType_INPROGRESS);
			}
			else if (progressStr == "Completable")
			{
				qs->set_progresstype(SaveObject::QuestStatus_QuestProgressType_COMPLETABLE);
			}
			else if (progressStr == "Completed")
			{
				qs->set_progresstype(SaveObject::QuestStatus_QuestProgressType_COMPLETED);
			}

			qs->set_currphase(set->getUInt("currPhase"));
			qs->set_completed(set->getUInt("completed"));
		}
		else
		{
			auto sqs = outPlayerInfo.mutable_queststatus(set->getUInt("questIdx"))->add_substatus();
			sqs->set_bstarted(set->getBoolean("bStarted"));
			sqs->set_bcompleted(set->getBoolean("bCompleted"));
			sqs->set_curramount(set->getUInt("currAmount"));
		}
	}
}

void GameServer::MakeDefaultPlayerInfo(SaveObject::PlayerInfo& outPlayerInfo)
{
	outPlayerInfo.set_level(1);
	outPlayerInfo.set_map(0);
	outPlayerInfo.set_loc_x(0.f);
	outPlayerInfo.set_loc_y(0.f);
	outPlayerInfo.set_loc_z(-100.f);
	outPlayerInfo.set_gold(0);

	// Inventory
	for (int i = 0; i < SaveObject::Inventory_ItemType_ItemType_ARRAYSIZE; ++i)
	{
		outPlayerInfo.mutable_inventory()->add_typeinventory();
	}

	// Quest Status
	ifstream listPathStream("../Contents/Data/Quest.json");

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;

	JSONCPP_STRING err;
	Json::Value val;
	
	Json::parseFromStream(builder, listPathStream, &val, &err);

	if (err.empty() == false)
	{
		MessageBoxA(nullptr, err.c_str(), "Error!", MB_OK);
	}

	for (int i = 0; i < val["QuestList"].size(); ++i)
	{
		SaveObject::QuestStatus* qs = outPlayerInfo.add_queststatus();
		qs->set_index(i);
		qs->set_type((val["QuestList"][i]["Type"].asString() == "Serial") ? SaveObject::QuestStatus_QuestType_SERIAL : SaveObject::QuestStatus_QuestType_PARALLEL);
		qs->set_currphase(0);
		qs->set_completed(0);
		qs->set_progresstype(SaveObject::QuestStatus_QuestProgressType_AVAILABLE);	//TODO : ����Ʈ ���� ���� ����� �������� ����
		
		for (int j = 0; j < val["QuestList"][i]["SubQuests"].size(); ++j)
		{
			const Json::Value subQuestVal = val["QuestList"][i]["SubQuests"][j];
			SaveObject::QuestStatus_SubQuestStatus* sqs = qs->add_substatus();
			sqs->set_bstarted(false);
			sqs->set_bcompleted(false);
			sqs->set_curramount(0);

			string typeStr = subQuestVal["Type"].asString();
			if (typeStr == "Arrival")
			{
				sqs->set_type(SaveObject::QuestStatus_SubQuestStatus_SubQuestType_ARRIVAL);
			}
			else if (typeStr == "Hunt")
			{
				sqs->set_type(SaveObject::QuestStatus_SubQuestStatus_SubQuestType_HUNT);
			}
			else if (typeStr == "Item")
			{
				sqs->set_type(SaveObject::QuestStatus_SubQuestStatus_SubQuestType_ITEM);
			}
			else if (typeStr == "Action")
			{
				sqs->set_type(SaveObject::QuestStatus_SubQuestStatus_SubQuestType_ACTION);
			}
		}
	}
}

void GameServer::SavePlayerInfo(const SaveObject::PlayerInfo& playerInfo, int userIdx)
{
	unique_ptr<sql::PreparedStatement> pstmt(mSQLConnection->prepareStatement("CALL SavePlayerInfo(?, ?, ?, ?, ?, ?, ?, ?)"));
	pstmt->setInt(0, userIdx);
	pstmt->setString(1, playerInfo.name());
	pstmt->setUInt(2, playerInfo.level());
	pstmt->setUInt(3, playerInfo.map());
	pstmt->setDouble(4, playerInfo.loc_x());
	pstmt->setDouble(5, playerInfo.loc_y());
	pstmt->setDouble(6, playerInfo.loc_z());
	pstmt->setUInt64(7, playerInfo.gold());

	pstmt->executeQuery();

	// Inventory
	for (int i = 0; i < playerInfo.inventory().typeinventory_size(); ++i)
	{
		const auto& typeInventory = playerInfo.inventory().typeinventory(i);
		for (int j = 0; j < typeInventory.items_size(); ++j)
		{
			const auto& item = typeInventory.items(j);
			pstmt.reset(mSQLConnection->prepareStatement("CALL SaveInventory(?, ?, ?, ?, ?)"));
			pstmt->setInt(0, userIdx);

			switch (j)
			{
			case 0:
				pstmt->setString(1, "CLOTH");
				break;
			case 1:
				pstmt->setString(1, "WEAPON");
				break;
			case 2:
				pstmt->setString(2, "ITEM");
				break;
			default:
				continue;
			}

			pstmt->setUInt(2, j);
			pstmt->setUInt64(3, item.index());
			pstmt->setUInt64(4, item.num());

			pstmt->executeQuery();
		}
	}

	// Quest Status
	for (int i = 0; i < playerInfo.queststatus_size(); ++i)
	{
		const auto& qs = playerInfo.queststatus(i);

		for (int j = 0; j < playerInfo.queststatus(i).substatus_size(); ++j)
		{
			const auto& sqs = qs.substatus(j);
			pstmt.reset(mSQLConnection->prepareStatement("CALL SaveQueststatus(?, ?, ?, ?, ?, ?, ?, ?, ?)"));
			pstmt->setInt(0, userIdx);
			pstmt->setInt(1, qs.index());
			
			SaveObject::QuestStatus_QuestProgressType progress = qs.progresstype();
			switch (progress)
			{
			case SaveObject::QuestStatus_QuestProgressType_UNAVAILABLE:
				pstmt->setString(2, "Unavailable");
				break;
			case SaveObject::QuestStatus_QuestProgressType_AVAILABLE:
				pstmt->setString(2, "Available");
				break;
			case SaveObject::QuestStatus_QuestProgressType_INPROGRESS:
				pstmt->setString(2, "InProgress");
				break;
			case SaveObject::QuestStatus_QuestProgressType_COMPLETABLE:
				pstmt->setString(2, "Completable");
				break;
			case SaveObject::QuestStatus_QuestProgressType_COMPLETED:
				pstmt->setString(2, "Completed");
				break;
			default:
				continue;
			}

			pstmt->setInt(3, j);
			pstmt->setInt(4, qs.currphase());
			pstmt->setInt(5, qs.completed());
			pstmt->setBoolean(6, sqs.bstarted());
			pstmt->setBoolean(7, sqs.bcompleted());
			pstmt->setInt(8, sqs.curramount());

			pstmt->executeQuery();
		}
	}
}

bool GameServer::SetNickname(const char* nickname, int userIdx)
{
	unique_ptr<sql::PreparedStatement> pstmt(mSQLConnection->prepareStatement("CALL SetNickname(?, ?)"));
	pstmt->setInt(0, userIdx);
	pstmt->setString(1, nickname);

	auto result = pstmt->executeQuery();

	if (result->next())
	{
		return result->getBoolean("Succeeded");
	}
	else
	{
		return false;
	}
}

int main(int argc, char argv[])
{
	GameServer* server = new GameServer;

	// WSA �ʱ�ȭ
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