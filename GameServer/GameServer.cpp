#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "GameServer.h"
#include <iostream>
#include <fstream>
#include <json.h>
#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using ProtoObject::GameService;
using ProtoObject::LoginInfo;
using ProtoObject::PlayerInfo;

gRPCService::gRPCService()
{
	// DB 연결
	mSQLDriver = sql::mysql::get_mysql_driver_instance();
	mSQLConnection.reset(mSQLDriver->connect("tcp://127.0.0.1:3306", "root", "redapple67#"));
	mSQLConnection->setSchema("portfolio");

	cout << "Database connected" << endl;
}

Status gRPCService::TryLogin(ServerContext* context, const LoginInfo* li, PlayerInfo* pi)
{
	// 입력 파싱
	const string id = li->id();
	const string pw = li->password();
	const string client = context->peer();

	cout << "Login asked from Client : " << client << endl;

	// DB에서 유저 정보를 확인하고, 저장된 데이터를 받아온다.
	unique_ptr<sql::PreparedStatement> pstmt = nullptr;
	unique_ptr<sql::ResultSet> sqlres = nullptr;

	// SP 실행
	pstmt.reset(mSQLConnection->prepareStatement("CALL TryLogin(?, ?)"));
	pstmt->setString(1, id);
	pstmt->setString(2, pw);
	sqlres.reset(pstmt->executeQuery());
	Reconnect();	// TEMP : 여러 쿼리문을 실행할 경우, 크래시 발생. 임시로 한번 실행할때마다 DB를 재연결하도록 조치

	// SP 결과
	if (sqlres->next())
	{
		int userIdx = sqlres->getInt("userIdx");	// 로그인이 성공하면, 다른 table에 접근 가능한 유저 고유 번호를 반환
		if (userIdx < 0)
		{
			// 로그인 실패
			cout << "Login Failed : " << client << endl;
			pi->set_map(10000);	// 로그인 실패를 클라이언트에 알림
		}
		else
		{
			cout << "Client " << client << "'s userIdx : " << userIdx << endl;

			// 클라이언트-userIdx를 map에 보관 (게임 상태를 저장할 때 쓰임)
			mClients.insert(make_pair(client, Client(userIdx)));

			// userIdx로 PlayerInfo 테이블의 데이터를 가져옴
			pstmt.reset(mSQLConnection->prepareStatement("CALL GetPlayerInfo(?)"));
			pstmt->setInt(1, userIdx);
			sqlres.reset(pstmt->executeQuery());
			Reconnect();
			MakePlayerInfoFromResultSet(sqlres.get(), pi);

			// userIdx로 Inventory 테이블의 데이터를 가져옴
			pstmt.reset(mSQLConnection->prepareStatement("CALL GetInventory(?)"));
			pstmt->setInt(1, userIdx);
			sqlres.reset(pstmt->executeQuery());
			Reconnect();
			MakeInventoryFromResultSet(sqlres.get(), pi);

			// userIdx로 QuestStatus 테이블의 데이터를 가져옴
			pstmt.reset(mSQLConnection->prepareStatement("CALL GetQuestStatus(?)"));
			pstmt->setInt(1, userIdx);
			sqlres.reset(pstmt->executeQuery());
			Reconnect();
			MakeQuestStatusFromResultSet(sqlres.get(), pi);

			pi->set_useridx(userIdx);
		}
	}

	return grpc::Status::OK;
}

Status gRPCService::TryRegister(ServerContext* context, const LoginInfo* li, PlayerInfo* pi)
{
	// 입력 파싱
	const string id = li->id();
	const string pw = li->password();
	const string client = context->peer();

	cout << "Register asked from Client : " << client << endl;

	unique_ptr<sql::PreparedStatement> pstmt = nullptr;
	unique_ptr<sql::ResultSet> sqlres = nullptr;

	// 회원가입을 시도하고, 성공 시 생성된 userIdx를 반환함
	pstmt.reset(mSQLConnection->prepareStatement("CALL TryRegister(?, ?)"));
	pstmt->setString(1, id);
	pstmt->setString(2, pw);

	sqlres.reset(pstmt->executeQuery());
	Reconnect();

	int userIdx;

	if (sqlres->next())
	{
		userIdx = sqlres->getInt("userIdx");
		if (userIdx < 0)	// 회원가입 실패 (중복 계정)
		{
			pi->set_map(10000);	// 비정상 값을 넣어 실패를 알림
			return grpc::Status::OK;
		}

		mClients.insert(make_pair(client, Client(userIdx)));
	}
	else
	{
		return grpc::Status::CANCELLED;
	}

	// 기본 플레이어 데이터 생성
	MakeDefaultPlayerInfo(pi);

	pi->set_useridx(userIdx);

	// DB에 저장
	return Save(context, pi, nullptr);
}

Status gRPCService::SetNickname(ServerContext* context, const Nickname* in, Nickname* out)
{
	// 닉네임 설정은 회원가입 후에 이루어지기 때문에, Client map에 userIdx가 저장되어 있을 것
	auto found = mClients.find(context->peer());
	if (found == mClients.end())
	{
		return grpc::Status::CANCELLED;
	}

	const int userIdx = found->second.userIdx;

	// 닉네임 설정 SP 실행
	unique_ptr<sql::PreparedStatement> pstmt(mSQLConnection->prepareStatement("CALL SetNickname(?, ?)"));
	pstmt->setInt(1, userIdx);
	pstmt->setString(2, in->nickname());

	auto result = pstmt->executeQuery();
	Reconnect();

	if (!result->next())
	{
		return grpc::Status::CANCELLED;
	}

	if (result->getBoolean("Succeeded") == false)	// 닉네임 설정 실패 (닉네임 중복)
	{
		out->set_nickname("failed");
	}
	else
	{
		out->set_nickname(in->nickname());	// 설정 성공 시, 에코 메시지 전송
	}

	return grpc::Status::OK;
}

Status gRPCService::Save(ServerContext* context, const PlayerInfo* pi, Empty* empty)
{
	// 로그인 후에만 저장이 가능하므로, Client Map 내에 userIdx가 존재할 것
	auto found = mClients.find(context->peer());
	if (found == mClients.end())
	{
		return grpc::Status::CANCELLED;
	}

	const int userIdx = found->second.userIdx;

	// PlayerInfo를 DB에 저장 (Inventory, QuestStatus는 별도의 SP로 구분)
	unique_ptr<sql::PreparedStatement> pstmt(mSQLConnection->prepareStatement("CALL SavePlayerInfo(?, ?, ?, ?, ?, ?, ?, ?)"));
	pstmt->setInt(1, userIdx);
	pstmt->setString(2, pi->name());
	pstmt->setUInt(3, pi->level());
	pstmt->setUInt(4, pi->map());
	pstmt->setDouble(5, pi->loc_x());
	pstmt->setDouble(6, pi->loc_y());
	pstmt->setDouble(7, pi->loc_z());
	pstmt->setUInt64(8, pi->gold());

	pstmt->executeQuery();
	Reconnect();

	// Inventory 목록을 DB에 추가/업데이트
	for (int i = 0; i < pi->inventory().typeinventory_size(); ++i)
	{
		const auto& typeInventory = pi->inventory().typeinventory(i);
		for (int j = 0; j < typeInventory.items_size(); ++j)
		{
			const auto& item = typeInventory.items(j);
			pstmt.reset(mSQLConnection->prepareStatement("CALL SaveInventory(?, ?, ?, ?, ?)"));
			pstmt->setInt(1, userIdx);

			switch (i)
			{
			case 0:
				pstmt->setString(2, "CLOTH");
				break;
			case 1:
				pstmt->setString(2, "WEAPON");
				break;
			case 2:
				pstmt->setString(2, "ITEM");
				break;
			default:
				continue;
			}

			pstmt->setUInt(3, j);
			pstmt->setUInt64(4, item.index());
			pstmt->setUInt64(5, item.num());

			pstmt->executeQuery();
			Reconnect();
		}
	}

	// Quest Status 목록을 DB에 추가/업데이트
	for (int i = 0; i < pi->queststatus_size(); ++i)
	{
		const auto& qs = pi->queststatus(i);

		for (int j = 0; j < pi->queststatus(i).substatus_size(); ++j)
		{
			const auto& sqs = qs.substatus(j);
			pstmt.reset(mSQLConnection->prepareStatement("CALL SaveQueststatus(?, ?, ?, ?, ?, ?, ?, ?, ?)"));
			pstmt->setInt(1, userIdx);
			pstmt->setInt(2, qs.index());

			ProtoObject::QuestStatus_QuestProgressType progress = qs.progresstype();
			switch (progress)
			{
			case ProtoObject::QuestStatus_QuestProgressType_UNAVAILABLE:
				pstmt->setString(3, "Unavailable");
				break;
			case ProtoObject::QuestStatus_QuestProgressType_AVAILABLE:
				pstmt->setString(3, "Available");
				break;
			case ProtoObject::QuestStatus_QuestProgressType_INPROGRESS:
				pstmt->setString(3, "InProgress");
				break;
			case ProtoObject::QuestStatus_QuestProgressType_COMPLETABLE:
				pstmt->setString(3, "Completable");
				break;
			case ProtoObject::QuestStatus_QuestProgressType_COMPLETED:
				pstmt->setString(3, "Completed");
				break;
			default:
				continue;
			}

			pstmt->setInt(4, j);
			pstmt->setInt(5, qs.currphase());
			pstmt->setInt(6, qs.completed());
			pstmt->setBoolean(7, sqs.bstarted());
			pstmt->setBoolean(8, sqs.bcompleted());
			pstmt->setInt(9, sqs.curramount());

			pstmt->executeQuery();
			Reconnect();
		}
	}

	return grpc::Status::OK;
}

Status gRPCService::SendLocation(ServerContext* context, ServerReader<Location>* reader, Empty* empty)
{
	string clientTag = context->peer();
	Location request;
	while (reader->Read(&request))
	{
		lock_guard<mutex> lock(_mu);
		mClients[clientTag].location = request;	// 위치 정보 갱신

		for (auto& pair : mClients)
		{
			const auto& client = pair.second;
			// 같은 맵에 위치한 유저(클라이언트)들에 위치 정보 전송
			if (client.locationWriter && (client.currentMapIdx == mClients[clientTag].currentMapIdx))
			{
				client.locationWriter->Write(request);
			}
		}
	}

	return Status::OK;
}

Status gRPCService::SendRepBoolean(ServerContext* context, ServerReader<RepBoolean>* reader, Empty* empty)
{
	string clientTag = context->peer();
	RepBoolean request;
	while (reader->Read(&request))
	{
		lock_guard<mutex> lock(_mu);

		// 정보 갱신
		switch (request.type())
		{
		case ProtoObject::RepBoolean_RepType_RUNNING:
			mClients[clientTag].bRunning = request.boolean();
			break;
		case ProtoObject::RepBoolean_RepType_JUMPING:
			mClients[clientTag].bJumping = request.boolean();
			break;
		case ProtoObject::RepBoolean_RepType_EQUIPPPED:
			mClients[clientTag].bEquipped = request.boolean();
			break;
		default:
			break;
		}

		for (auto & pair : mClients)
		{
			const auto& client = pair.second;
			// 같은 맵에 위치한 유저(클라이언트)들에 전송
			if (client.repBooleanWriter && (client.currentMapIdx == mClients[clientTag].currentMapIdx))
			{
				client.repBooleanWriter->Write(request);
			}
		}
	}
	return Status::OK;
}

Status gRPCService::SendEquipmentChange(ServerContext* context, const Equipment* request, Empty* empty)
{
	string clientTag = context->peer();

	{
		lock_guard<mutex> lock(_mu);

		// 정보 갱신
		mClients[clientTag].equipment = *request;

		for (auto& pair : mClients)
		{
			const auto& client = pair.second;
			// 같은 맵에 위치한 유저(클라이언트)들에 전송
			if (client.equipmentWriter && (client.currentMapIdx == mClients[clientTag].currentMapIdx))
			{
				client.equipmentWriter->Write(*request);
			}
		}
	}
	return Status::OK;
}

Status gRPCService::SendMapResourceChange(ServerContext* context, const ResourceChange* request, Empty* empty)
{
	string clientTag = context->peer();

	// MapState 갱신
	if (request->restype() == ProtoObject::ResourceChange_ResourceType_ENEMY)
	{
		if (request->reschangetype() == ProtoObject::ResourceChange_ChangeType_REMOVE)
		{
			DeleteEnemy(mClients[clientTag].currentMapIdx, request->residx());
		}
	}
	else
	{
		if (request->reschangetype() == ProtoObject::ResourceChange_ChangeType_REMOVE)
		{
			DeleteItem(mClients[clientTag].currentMapIdx, request->residx());
		}
	}

	// Broadcast
	lock_guard<mutex> lock(_mu);
	for (auto& pair : mClients)
	{
		const auto& client = pair.second;
		// 해당 맵에 위치한 유저(클라이언트)들에 전송
		if (client.resourceChangeWriter && (client.currentMapIdx == mClients[clientTag].currentMapIdx))
		{
			client.resourceChangeWriter->Write(*request);
		}
	}

	return Status::OK;
}

Status gRPCService::SendMapTransition(ServerContext* context, const MapTransition* request, Empty* empty)
{
	string clientTag = context->peer();
	Client* caller = &mClients[clientTag];
	caller->currentMapIdx = request->after();	// 정보 갱신

	// Broadcast
	lock_guard<mutex> lock(_mu);
	for (auto& pair : mClients)
	{
		const auto& client = pair.second;
		if (client.mapTransitionWriter)
		{
			// 이탈한 맵의 유저들에게 정보 전송 (해당 유저 캐릭터를 삭제해야 함)
			if (client.currentMapIdx == request->before())
			{
				client.mapTransitionWriter->Write(*request);
			}
			// 1. 진입하는 맵의 유저들에게 정보 전송 (해당 유저 캐릭터를 스폰해야 함)
			// 2. 진입하는 유저에게는 기존 맵 유저들의 정보 전송 (위치, 애니메이션, 장비)
			// 3. 기존 맵 유저들에게는 진입하는 유저의 정보 전송 (장비) 
			else if (client.currentMapIdx == request->after())
			{
				client.mapTransitionWriter->Write(*request);

				caller->locationWriter->Write(client.location);

				RepBoolean runBoolean;
				runBoolean.set_useridx(client.userIdx);
				runBoolean.set_type(ProtoObject::RepBoolean_RepType_RUNNING);
				runBoolean.set_boolean(client.bRunning);
				caller->repBooleanWriter->Write(runBoolean);

				RepBoolean jumpBoolean;
				jumpBoolean.set_useridx(client.userIdx);
				jumpBoolean.set_type(ProtoObject::RepBoolean_RepType_JUMPING);
				jumpBoolean.set_boolean(client.bJumping);
				caller->repBooleanWriter->Write(jumpBoolean);

				RepBoolean equipBoolean;
				equipBoolean.set_useridx(client.userIdx);
				equipBoolean.set_type(ProtoObject::RepBoolean_RepType_EQUIPPPED);
				equipBoolean.set_boolean(client.bEquipped);
				caller->repBooleanWriter->Write(equipBoolean);

				caller->equipmentWriter->Write(client.equipment);

				// 기존 클라이언트에 caller 정보 전송
				// 맵 이동 직후에 running, jumping 상태일 수 없으므로 전송 X
				RepBoolean equipBoolean_c;
				equipBoolean_c.set_useridx(caller->userIdx);
				equipBoolean_c.set_type(ProtoObject::RepBoolean_RepType_EQUIPPPED);
				equipBoolean_c.set_boolean(caller->bEquipped);
				client.repBooleanWriter->Write(equipBoolean_c);
				
				client.equipmentWriter->Write(caller->equipment);
			}
		}
	}

	return Status::OK;
}

Status gRPCService::BroadcastLocation(ServerContext* context, const Empty* empty, ServerWriter<Location>* writer)
{
	{
		lock_guard<mutex> lock(_mu);
		mClients[context->peer()].locationWriter = writer;
	}

	// Writer 객체를 계속 사용하기 위해 스레드를 종료시키지 않음.
	while (true)
	{
		this_thread::sleep_for(chrono::seconds(1));
	}

	return Status::OK;
}

Status gRPCService::BroadcastRepBoolean(ServerContext* context, const Empty* empty, ServerWriter<RepBoolean>* writer)
{
	{
		lock_guard<mutex> lock(_mu);
		mClients[context->peer()].repBooleanWriter = writer;
	}

	// Writer 객체를 계속 사용하기 위해 스레드를 종료시키지 않음.
	while (true)
	{
		this_thread::sleep_for(chrono::seconds(1));
	}

	return Status::OK;
}

Status gRPCService::BroadcastEquipmentChange(ServerContext* context, const Empty* empty, ServerWriter<Equipment>* writer)
{
	{
		lock_guard<mutex> lock(_mu);
		mClients[context->peer()].equipmentWriter = writer;
	}

	// Writer 객체를 계속 사용하기 위해 스레드를 종료시키지 않음.
	while (true)
	{
		this_thread::sleep_for(chrono::seconds(1));
	}

	return Status::OK;
}

Status gRPCService::BroadcastMapResourceChange(ServerContext* context, const Empty* empty, ServerWriter<ResourceChange>* writer)
{
	{
		lock_guard<mutex> lock(_mu);
		mClients[context->peer()].resourceChangeWriter = writer;
	}

	// Writer 객체를 계속 사용하기 위해 스레드를 종료시키지 않음.
	while (true)
	{
		this_thread::sleep_for(chrono::seconds(1));
	}

	return Status::OK;
}

Status gRPCService::BroadcastMapTransition(ServerContext* context, const Empty* empty, ServerWriter<MapTransition>* writer)
{
	{
		lock_guard<mutex> lock(_mu);
		mClients[context->peer()].mapTransitionWriter = writer;
	}

	// Writer 객체를 계속 사용하기 위해 스레드를 종료시키지 않음.
	while (true)
	{
		this_thread::sleep_for(chrono::seconds(1));
	}

	return Status::OK;
}

void gRPCService::InitMapState()
{
	ifstream levelInfoStream("C:/Users/User/Documents/Unreal Projects/Portfolio/Content/Data/LevelInfo.json");	// TEMP : 절대경로
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;

	JSONCPP_STRING err;
	Json::Value val;

	Json::parseFromStream(builder, levelInfoStream, &val, &err);

	if (err.empty() == false)
	{
		MessageBoxA(nullptr, err.c_str(), "Error!", MB_OK);
	}

	mMapState.resize(val["LevelInfoList"].size());
	for (int i = 0; i < val["LevelInfoList"].size(); ++i)
	{
		mMapState[i].mapIdx = i;

		const string mapName = val["LevelInfoList"][i]["Name"].asString();
		const string levelDataPath("C:/Users/User/Documents/Unreal Projects/Portfolio/Content/Data/LevelData/" + mapName + "/");

		ifstream enemyDataStream(levelDataPath + "Enemy.json");
		Json::parseFromStream(builder, enemyDataStream, &val, &err);
		if (err.empty() == false)
		{
			MessageBoxA(nullptr, err.c_str(), "Error!", MB_OK);
		}

		mMapState[i].enemyState.resize(val["EnemyList"].size(), true);
		
		ifstream itemDataStream(levelDataPath + "Item.json");
		Json::parseFromStream(builder, itemDataStream, &val, &err);
		if (err.empty() == false)
		{
			MessageBoxA(nullptr, err.c_str(), "Error!", MB_OK);
		}

		mMapState[i].itemState.resize(val["ItemList"].size(), true);
	}
}

void gRPCService::DeleteEnemy(int mapIdx, int enemyIdx)
{
	mMapState[mapIdx].enemyState[enemyIdx] = false;
}

void gRPCService::DeleteItem(int mapIdx, int itemIdx)
{
	mMapState[mapIdx].itemState[itemIdx] = false;
}

void gRPCService::MakePlayerInfoFromResultSet(sql::ResultSet* set, ProtoObject::PlayerInfo* outPlayerInfo)
{
	if (set->next())
	{
		outPlayerInfo->set_name(set->getString("nickname"));
		outPlayerInfo->set_level(set->getUInt("playerLevel"));
		outPlayerInfo->set_map(set->getUInt("mapIdx"));
		outPlayerInfo->set_loc_x(set->getDouble("loc_x"));
		outPlayerInfo->set_loc_y(set->getDouble("loc_y"));
		outPlayerInfo->set_loc_z(set->getDouble("loc_z"));
		outPlayerInfo->set_gold(set->getUInt("gold"));
	}
}

void gRPCService::MakeInventoryFromResultSet(sql::ResultSet* set, ProtoObject::PlayerInfo* outPlayerInfo)
{
	ProtoObject::Inventory* outInventory = outPlayerInfo->mutable_inventory();

	// 타입별 인벤토리 생성
	for (int i = 0; i < ProtoObject::Inventory_ItemType_ItemType_ARRAYSIZE; ++i)
	{
		outInventory->add_typeinventory();
	}

	// TypeInventory에 각 아이템 추가
	while (set->next())
	{
		string typeStr = set->getString("itemType");

		ProtoObject::GameItem* item = nullptr;

		if (typeStr == "CLOTH")
		{
			item = outInventory->mutable_typeinventory(0)->add_items();
		}
		else if (typeStr == "WEAPON")
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

void gRPCService::MakeQuestStatusFromResultSet(sql::ResultSet* set, ProtoObject::PlayerInfo* outPlayerInfo)
{
	// ProtoObject::QuestStatus : 메인-서브퀘스트 2차원 배열로 구성되어 있음.
		// QuestStatus
			// progress, ...
			// SubQuestStatus
				// -- progress, ...
	
	// DB 내의 QuestStatus : 각 row가 서브퀘의 상태를 보관 ( + 메인퀘의 idx, 상태까지 가짐)
		// Row : (메인퀘 idx, 메인퀘 progress, 서브퀘 idx, 서브퀘 progress, ...)

	while (set->next())
	{
		const int qIdx = set->getUInt("questIdx");

		if (qIdx >= outPlayerInfo->queststatus_size())
		{
			for (int i = 0; i <= qIdx; ++i)
			{
				outPlayerInfo->add_queststatus();
			}
		}

		auto qs = outPlayerInfo->mutable_queststatus(qIdx);
		// 메인퀘 정보는 각 서브퀘 row가 모두 같은 값을 공유하므로, 한번만 적용하면 된다.
		if (qs->index() != qIdx)
		{
			qs->set_index(set->getUInt("questIdx"));

			string progressStr = set->getString("progress");
			if (progressStr == "Unavailable")
			{
				qs->set_progresstype(ProtoObject::QuestStatus_QuestProgressType_UNAVAILABLE);
			}
			else if (progressStr == "Available")
			{
				qs->set_progresstype(ProtoObject::QuestStatus_QuestProgressType_AVAILABLE);
			}
			else if (progressStr == "InProgress")
			{
				qs->set_progresstype(ProtoObject::QuestStatus_QuestProgressType_INPROGRESS);
			}
			else if (progressStr == "Completable")
			{
				qs->set_progresstype(ProtoObject::QuestStatus_QuestProgressType_COMPLETABLE);
			}
			else if (progressStr == "Completed")
			{
				qs->set_progresstype(ProtoObject::QuestStatus_QuestProgressType_COMPLETED);
			}

			qs->set_currphase(set->getUInt("currPhase"));
			qs->set_completed(set->getUInt("completed"));
		}

		// 서브퀘 정보 적용
		const int sqIdx = set->getUInt("subquestIdx");
		if (sqIdx >= qs->substatus_size())
		{
			for (int i = 0; i <= sqIdx; ++i)
			{
				qs->add_substatus();
			}
		}

		auto sqs = qs->mutable_substatus(sqIdx);
		sqs->set_bstarted(set->getBoolean("bStarted"));
		sqs->set_bcompleted(set->getBoolean("bCompleted"));
		sqs->set_curramount(set->getUInt("currAmount"));
	}
}

void gRPCService::MakeDefaultPlayerInfo(ProtoObject::PlayerInfo* outPlayerInfo)
{
	outPlayerInfo->set_level(1);
	outPlayerInfo->set_map(0);
	outPlayerInfo->set_loc_x(0.f);
	outPlayerInfo->set_loc_y(0.f);
	outPlayerInfo->set_loc_z(-100.f);
	outPlayerInfo->set_gold(0);

	// 타입별 인벤토리 생성
	for (int i = 0; i < ProtoObject::Inventory_ItemType_ItemType_ARRAYSIZE; ++i)
	{
		outPlayerInfo->mutable_inventory()->add_typeinventory();
	}

	// 게임데이터 내의 Quest.json에 접근해 QuestStatus 생성 (Quest 타입 등)
	ifstream listPathStream("C:/Users/User/Documents/Unreal Projects/Portfolio/Content/Data/Quest.json");	// TEMP : 절대경로
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
		ProtoObject::QuestStatus* qs = outPlayerInfo->add_queststatus();
		qs->set_index(i);
		qs->set_type((val["QuestList"][i]["Type"].asString() == "Serial") ? ProtoObject::QuestStatus_QuestType_SERIAL : ProtoObject::QuestStatus_QuestType_PARALLEL);
		qs->set_currphase(0);
		qs->set_completed(0);
		qs->set_progresstype(ProtoObject::QuestStatus_QuestProgressType_AVAILABLE);

		for (int j = 0; j < val["QuestList"][i]["SubQuests"].size(); ++j)
		{
			const Json::Value subQuestVal = val["QuestList"][i]["SubQuests"][j];
			ProtoObject::QuestStatus_SubQuestStatus* sqs = qs->add_substatus();
			sqs->set_bstarted(false);
			sqs->set_bcompleted(false);
			sqs->set_curramount(0);

			string typeStr = subQuestVal["Type"].asString();
			if (typeStr == "Arrival")
			{
				sqs->set_type(ProtoObject::QuestStatus_SubQuestStatus_SubQuestType_ARRIVAL);
			}
			else if (typeStr == "Hunt")
			{
				sqs->set_type(ProtoObject::QuestStatus_SubQuestStatus_SubQuestType_HUNT);
			}
			else if (typeStr == "Item")
			{
				sqs->set_type(ProtoObject::QuestStatus_SubQuestStatus_SubQuestType_ITEM);
			}
			else if (typeStr == "Action")
			{
				sqs->set_type(ProtoObject::QuestStatus_SubQuestStatus_SubQuestType_ACTION);
			}
		}
	}
}

bool gRPCService::Reconnect()
{
	mSQLConnection->close();
	mSQLConnection.reset(mSQLDriver->connect("tcp://127.0.0.1:3306", "root", "redapple67#"));
	if (!mSQLConnection)
	{
		return false;
	}
	mSQLConnection->setSchema("portfolio");
	return true;
}

void gRPCService::ListClients()
{
	for (auto iter : mClients)
	{
		cout << iter.first << " : " << iter.second.userIdx << endl;
	}
}

int main()
{
	string serverAddr = "localhost:5050";
	gRPCService service;

	grpc::ServerBuilder builder;
	builder.AddListeningPort(serverAddr, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	unique_ptr<grpc::Server> server(builder.BuildAndStart());
	
	cout << "Server Listening" << endl;
	
	thread waiter(&grpc::Server::Wait, server.get());

	string command;
	while (true)
	{
		cin >> command;
		if (command == "q")
		{
			cout << "Command : Server Shutdown" << endl;
			server->Shutdown();
			break;
		}
		else if (command == "l")
		{
			cout << "Command : List Clients" << endl;
			service.ListClients();
		}
		else
		{
			cout << "Invalid Command" << endl;
		}
	}

	waiter.join();

	return 0;
}
