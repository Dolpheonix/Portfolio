#pragma once

#include <string>
#include "ProtoObject/ProtoObject.grpc.pb.h"
#include "ProtoObject/ProtoObject.pb.h"
#include "../Core/PlayerSaveObject.h"
#include "../Common/GameObject.h"

using namespace std;

using google::protobuf::Empty;
using grpc::Channel;
using grpc::CompletionQueue;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientWriter;
using grpc::Status;
using grpc::ClientAsyncWriter;
using grpc::ClientAsyncReader;

using ProtoObject::GameService;
using ProtoObject::Location;
using ProtoObject::RepBoolean;
using ProtoObject::Equipment;
using ProtoObject::ResourceChange;
using ProtoObject::MapTransition;

/*
	GameServer와 연결되는 클라이언트 클래스
	- Protobuf/gRpc 방식으로 서버와 데이터를 주고받음
*/

class UCustomGameInstance;

class MainClient
{
private:
	MainClient();
	MainClient(const MainClient& rhs) = delete;
	MainClient& operator=(const MainClient& rhs) = delete;
	~MainClient() {}
public:
	static MainClient* GetSingleton();
	// 로그인 시도 
	PlayerInfo TryLogin(const string& id, const string& pw);	
	// 회원가입 시도
	PlayerInfo TryRegister(const string& id, const string& pw);
	// 닉네임 설정
	string SetNickname(const string& nickname);		
	// 현재 게임 상태 저장
	bool Save(const PlayerInfo toSave);							

	// 위치,회전 정보 전송
	void SendLocation(const GameObject::Location& loc);
	// 스트림 Writer를 할당받음
	void ReceiveLocation();

	// 애니메이션용 Boolean 값 전송
	void SendRepBoolean(const GameObject::RepBoolean& request);
	// 스트림 Writer를 할당받음
	void ReceiveRepBoolean();

	// 장비 변경 정보를 전송
	void SendEquipmentChange(const GameObject::Equipment& request);
	// 스트림 Writer를 할당받음
	void ReceiveEquipmentChange();

	// 맵 리소스 변화를 전송
	void SendMapResourceChange(const GameObject::ResourceChange& request);
	// 스트림 Writer를 할당받음
	void ReceiveMapResourceChange();

	// 맵 이탈/진입을 전송
	void SendMapTransition(const GameObject::MapTransition& request);
	// 스트림 Writer를 할당받음
	void ReceiveMapTransition();

	// gRpc channel, stub 생성
	void RunClient(TObjectPtr<UCustomGameInstance> gi);	

public:
	unique_ptr<ProtoObject::GameService::Stub> mStub;	// 클라이언트 Stub

	unique_ptr<ClientWriter<ProtoObject::Location>> mLocationWriter;		// Location 정보 전송용 Writer
	unique_ptr<ClientReader<ProtoObject::Location>> mLocationReader;		// Location 정보 수신용 Reader

	unique_ptr<ClientWriter<ProtoObject::RepBoolean>> mRepBooleanWriter;	// RepBoolean 정보 전송용 Writer
	unique_ptr<ClientReader<ProtoObject::RepBoolean>> mRepBooleanReader;	// RepBoolean 정보 수신용 Reader
	
	unique_ptr<ClientReader<ProtoObject::Equipment>> mEquipmentReader;			// Equipment 정보 수신용 Reader
	unique_ptr<ClientReader<ProtoObject::ResourceChange>> mResourceChangeReader;	// ResourceChange 정보 수신용 Reader
	unique_ptr<ClientReader<ProtoObject::MapTransition>> mMapTransitionReader;	// MapTransition 정보 수신용 Reader

	TObjectPtr<UCustomGameInstance> mGameInstance;	// 게임 인스턴스
};