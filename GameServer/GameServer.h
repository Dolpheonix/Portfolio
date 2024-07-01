#pragma once

#include <string>
#include <vector>
#include <mysql.h>
#include <mysql/jdbc.h>
#include "ProtoObject/ProtoObject.grpc.pb.h"
#include "ProtoObject/ProtoObject.pb.h"

/*
	GameServer
	1. Protobuf/gRPC를 통해 클라이언트에 로그인 관련 함수 제공
	2. 인게임 리플리케이션 함수 제공
	3. MYSQL DB를 통해 유저 정보를 저장/로드합니다.
*/

using namespace std;

using google::protobuf::Empty;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using ProtoObject::GameService;
using ProtoObject::LoginInfo;
using ProtoObject::PlayerInfo;
using ProtoObject::Nickname;
using ProtoObject::Location;
using ProtoObject::RepBoolean;
using ProtoObject::Equipment;
using ProtoObject::ResourceChange;
using ProtoObject::MapTransition;

// 클라이언트 정보
struct Client
{
	int userIdx;		// 현재 클라이언트의 유저 고유번호
	int currentMapIdx;	// 현재 위치한 맵의 인덱스
	Location location;	// 현재 위치
	// 애니메이션용 Boolean값
	bool bRunning;		// 달리는 상태인가?
	bool bJumping;		// 점프 상태인가?
	bool bEquipped;		// 무기 장착 상태인가?
	
	Equipment equipment; // 현재 장착한 무기/의상
	
	// gRpc 스트림 Writer (각 클라이언트에 정보 전달하는 용도)
	ServerWriter<Location>* locationWriter;				//위치 정보 전송
	ServerWriter<RepBoolean>* repBooleanWriter;			// 애니메이션 Boolean 전송
	ServerWriter<Equipment>* equipmentWriter;			// 장착 정보 전송
	ServerWriter<ResourceChange>* resourceChangeWriter;	// 맵의 리소스(루팅 아이템 / 몹) 변화 전송
	ServerWriter<MapTransition>* mapTransitionWriter;	// 특정 유저의 맵 이탈/진입 전송

	Client()	  : userIdx(-1), currentMapIdx(-1), location(Location()), locationWriter(nullptr),
					bRunning(false), bJumping(false), bEquipped(false),
					repBooleanWriter(nullptr), equipmentWriter(nullptr), resourceChangeWriter(nullptr), mapTransitionWriter(nullptr) {}
	
	Client(int u) : userIdx(u), currentMapIdx(-1), location(Location()), locationWriter(nullptr),
					bRunning(false), bJumping(false), bEquipped(false),
					repBooleanWriter(nullptr), equipmentWriter(nullptr), resourceChangeWriter(nullptr), mapTransitionWriter(nullptr) {}
};

// 각 맵의 리소스 현황을 관리하는 구조체
struct MapState
{
	int mapIdx;	// 맵 인덱스
	vector<bool> enemyState;	// 몹의 유효 상태
	vector<bool> itemState;		// 루팅 아이템의 유효 상태
};

// 메인 서버 클래스
// 1. 로그인/회원가입
// 2. 저장
// 3. 캐릭터 리플리케이션 (위치, 애니메이션, 장비)
// 4. 맵 리플리케이션 (아이템, 몹)
class gRPCService final : public GameService::Service
{
public:
	gRPCService();
	// 로그인 세션
	Status TryLogin(grpc::ServerContext* context, const LoginInfo* li, PlayerInfo* pi) override;	// 로그인 시도
	Status TryRegister(grpc::ServerContext* context, const LoginInfo* li, PlayerInfo* pi) override;	// 회원가입 시도
	Status SetNickname(grpc::ServerContext* context, const Nickname* in, Nickname* out) override;	// 닉네임 설정 (회원가입 성공 -> 닉네임 설정)
	Status Save(grpc::ServerContext* context, const PlayerInfo* pi, Empty* empty) override;	// 현재 상태 저장

	// 인게임 리플리케이션

	// 유저의 위치 정보를 같은 맵에 위치한 유저들에게 전송
	Status SendLocation(ServerContext* context, ServerReader<Location>* reader, Empty* empty) override;
	// 유저의 달리기/점프/장착 상태를 같은 맵에 위치한 유저들에게 전송
	Status SendRepBoolean(ServerContext* context, ServerReader<RepBoolean>* reader, Empty* empty) override;
	// 유저의 무기 변화를 같은 맵에 위치한 유저들에게 전송
	Status SendEquipmentChange(ServerContext* context, const Equipment* request, Empty* empty) override;
	// 맵의 몹/아이템 변화를 유저들에게 전송
	Status SendMapResourceChange(ServerContext* context, const ResourceChange* request, Empty* empty) override;
	// 유저의 맵 진입/이탈을 해당 맵에 위치한 유저들에게 전송
	Status SendMapTransition(ServerContext* context, const MapTransition* request, Empty* empty) override;

	// Writer 객체를 얻고, 스레드만 유지
	Status BroadcastLocation(ServerContext* context, const Empty* empty, ServerWriter<Location>* writer) override;
	Status BroadcastRepBoolean(ServerContext* context, const Empty* empty, ServerWriter<RepBoolean>* writer) override;
	Status BroadcastEquipmentChange(ServerContext* context, const Empty* empty, ServerWriter<Equipment>* writer) override;
	Status BroadcastMapResourceChange(ServerContext* context, const Empty* empty, ServerWriter<ResourceChange>* writer) override;
	Status BroadcastMapTransition(ServerContext* context, const Empty* empty, ServerWriter<MapTransition>* writer) override;
	
	// Map State 관련
	// 게임 데이터로 Map State 초기화
	void InitMapState();
	// enemyState 업데이트
	void DeleteEnemy(int mapIdx, int enemyIdx);
	// itemState 업데이트
	void DeleteItem(int mapIdx, int itemIdx);

	// DB 데이터 관련 함수
	// FetchResult에서 PlayerInfo 구조체 구성
	static void MakePlayerInfoFromResultSet(sql::ResultSet* set, PlayerInfo* outPlayerInfo);	
	// FetchResult에서 Inventory 구조체 구성
	static void MakeInventoryFromResultSet(sql::ResultSet* set, PlayerInfo* outPlayerInfo);	
	// FetchResult에서 QuestStatus 구조체 구성
	static void MakeQuestStatusFromResultSet(sql::ResultSet* set, PlayerInfo* outPlayerInfo);	
	// 회원가입 시, 최초 데이터 생성
	static void MakeDefaultPlayerInfo(PlayerInfo* outPlayerInfo);								
	bool Reconnect();	// DB와 재연결

	// 서버 명령어 함수
	// 접속된 클라이언트 나열
	void ListClients();

private:

	unordered_map<string, Client> mClients;		// 접속된 클라이언트 목록
	mutex _mu;									// mClients에 읽기/쓰기 시 사용
	
	// Server State
	vector<MapState> mMapState;	// 맵 리소스 상태

	// DB
	sql::mysql::MySQL_Driver* mSQLDriver;		// sql driver
	unique_ptr<sql::Connection> mSQLConnection;	// sql connection
};