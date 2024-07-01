#include "MainClient.h"
#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include "Async/Async.h"
#include "../Core/CustomGameInstance.h"

MainClient::MainClient()
{
}

MainClient* MainClient::GetSingleton()
{
	static MainClient mc;
	return &mc;
}

PlayerInfo MainClient::TryLogin(const string& id, const string& pw)
{
	// RPC용 구조체 생성
	ProtoObject::LoginInfo li;
	li.set_id(id);
	li.set_password(pw);

	// 출력 구조체
	ProtoObject::PlayerInfo pi = {};
	
	grpc::ClientContext context;
	grpc::Status res = mStub->TryLogin(&context, li, &pi);	// RPC 호출

	if (!res.ok())
	{
		// TOOD : error 처리
	}

	PlayerInfo outInfo = {};
	outInfo.ConvertFromProto(pi);	// 인게임용 구조체로 변환
	return outInfo;
}

PlayerInfo MainClient::TryRegister(const string& id, const string& pw)
{
	ProtoObject::LoginInfo li;
	li.set_id(id);
	li.set_password(pw);

	ProtoObject::PlayerInfo pi = {};

	grpc::ClientContext context;
	grpc::Status res = mStub->TryRegister(&context, li, &pi);

	if (!res.ok())
	{
		// error 처리
	}

	PlayerInfo outInfo = {};
	outInfo.ConvertFromProto(pi);
	return outInfo;
}

string MainClient::SetNickname(const string& nickname)
{
	ProtoObject::Nickname request;
	request.set_nickname(nickname);

	ProtoObject::Nickname reply;
	grpc::ClientContext context;
	grpc::Status res = mStub->SetNickname(&context, request, &reply);

	if (!res.ok())
	{
		// error 처리
	}

	return reply.nickname();
}

bool MainClient::Save(const PlayerInfo toSave)
{
	ProtoObject::PlayerInfo proto = {};
	toSave.ConvertToProto(proto);

	grpc::ClientContext context;
	grpc::Status res = mStub->Save(&context, proto, nullptr);

	if (!res.ok())
	{
		// error 처리
		return false;
	}

	return true;
}

void MainClient::SendLocation(const GameObject::Location& loc)
{
	ProtoObject::Location req = {};
	loc.ConvertToProto(req);

	mLocationWriter->Write(req);
}

void MainClient::ReceiveLocation()
{
	ProtoObject::Location rep = {};
	while (mLocationReader->Read(&rep))
	{
		cout << rep.useridx() << endl;
	}
}

void MainClient::SendRepBoolean(const GameObject::RepBoolean& request)
{
	ProtoObject::RepBoolean req = {};
	request.ConvertToProto(req);

	mRepBooleanWriter->Write(req);
}

void MainClient::ReceiveRepBoolean()
{
	ProtoObject::RepBoolean rep = {};
	while (mRepBooleanReader->Read(&rep))
	{
		// WHY? : 언리얼 객체를 사용하려면 게임 스레드 내에서 실행되야 함
		AsyncTask(ENamedThreads::GameThread, [this, rep]()
		{
			switch (rep.type())
			{
			case ProtoObject::RepBoolean_RepType_RUNNING:
				mGameInstance->ReplicateRunning(rep.useridx(), rep.boolean());
				break;
			case ProtoObject::RepBoolean_RepType_JUMPING:
				mGameInstance->ReplicateJumping(rep.useridx(), rep.boolean());
				break;
			case ProtoObject::RepBoolean_RepType_EQUIPPPED:
				mGameInstance->ReplicateEquipping(rep.useridx(), rep.boolean());
				break;
			default:
				break;
			}
		});
	}
}

void MainClient::SendEquipmentChange(const GameObject::Equipment& request)
{
	ProtoObject::Equipment req = {};
	request.ConvertToProto(req);
	
	ClientContext ctx;
	Empty empty;

	mStub->SendEquipmentChange(&ctx, req, &empty);
}

void MainClient::ReceiveEquipmentChange()
{
	ProtoObject::Equipment rep = {};
	while (mEquipmentReader->Read(&rep))
	{
		AsyncTask(ENamedThreads::GameThread, [this, rep]()
		{
			switch (rep.equiptype())
			{
			case ProtoObject::Equipment_EquipmentType_CLOTH:
				mGameInstance->ReplicateEquipmentChange(rep.useridx(), EItemType::Cloth, rep.itemindex());
				break;
			case ProtoObject::Equipment_EquipmentType_WEAPON:
				mGameInstance->ReplicateEquipmentChange(rep.useridx(), EItemType::Weapon, rep.itemindex());
				break;
			default:
				break;
			}
		});
	}
}

void MainClient::SendMapResourceChange(const GameObject::ResourceChange& request)
{
	ProtoObject::ResourceChange req = {};
	request.ConvertToProto(req);

	ClientContext ctx;
	Empty empty;

	mStub->SendMapResourceChange(&ctx, req, &empty);
}

void MainClient::ReceiveMapResourceChange()
{
	ProtoObject::ResourceChange rep = {};
	while (mResourceChangeReader->Read(&rep))
	{
		AsyncTask(ENamedThreads::GameThread, [this, rep]()
		{
			switch (rep.restype())
			{
			case ProtoObject::ResourceChange_ResourceType_ITEM:
				mGameInstance->ReplicateItemRemoval(rep.mapidx(), rep.residx());
				break;
			case ProtoObject::ResourceChange_ResourceType_ENEMY:
				mGameInstance->ReplicateEnemyRemoval(rep.mapidx(), rep.residx());
				break;
			default:
				break;
			}
		});
	}
}

void MainClient::SendMapTransition(const GameObject::MapTransition& request)
{
	ProtoObject::MapTransition req = {};
	request.ConvertToProto(req);

	ClientContext ctx;
	Empty empty;

	mStub->SendMapTransition(&ctx, req, &empty);
}

void MainClient::ReceiveMapTransition()
{
	ProtoObject::MapTransition rep = {};
	while (mMapTransitionReader->Read(&rep))
	{
		AsyncTask(ENamedThreads::GameThread, [this, rep]()
		{
			mGameInstance->ReplicateMapTransition(rep.useridx(), rep.before(), rep.after());
		});
	}
}

void MainClient::RunClient(TObjectPtr<UCustomGameInstance> gi)
{
	check(gi);

	auto channel = grpc::CreateChannel("127.0.0.1:5050", grpc::InsecureChannelCredentials());
	mStub = ProtoObject::GameService::NewStub(channel);

	ClientContext ctx_loc_w, ctx_loc_r, ctx_rep_w, ctx_rep_r, ctx_eq, ctx_rc, ctx_mt;
	Empty empty_loc_w, empty_loc_r, empty_rep_w, empty_rep_r, empty_eq, empty_rc, empty_mt;

	mLocationWriter = mStub->SendLocation(&ctx_loc_w, &empty_loc_w);
	mLocationReader = mStub->BroadcastLocation(&ctx_loc_r, empty_loc_r);

	mRepBooleanWriter = mStub->SendRepBoolean(&ctx_rep_w, &empty_rep_w);
	mRepBooleanReader = mStub->BroadcastRepBoolean(&ctx_rep_r, empty_rep_r);

	mEquipmentReader = mStub->BroadcastEquipmentChange(&ctx_eq, empty_eq);

	mResourceChangeReader = mStub->BroadcastMapResourceChange(&ctx_rc, empty_rc);

	mMapTransitionReader = mStub->BroadcastMapTransition(&ctx_mt, empty_mt);

	mGameInstance = gi;

	// Writer 스트림 객체를 유지시켜야 하므로, 각각 다른 스레드로 실행 후 루프 실행
	thread receiver_location(&MainClient::ReceiveLocation, this);
	thread receiver_repboolean(&MainClient::ReceiveRepBoolean, this);
	thread receiver_equipment(&MainClient::ReceiveEquipmentChange, this);
	thread receiver_resourceChange(&MainClient::ReceiveMapResourceChange, this);
	thread receiver_mapTransition(&MainClient::ReceiveMapTransition, this);

	receiver_location.join();
	receiver_repboolean.join();
	receiver_equipment.join();
	receiver_resourceChange.join();
	receiver_mapTransition.join();
}
