#include "GameObject.h"

using namespace GameObject;

void Location::ConvertFromProto(const ProtoObject::Location& proto)
{
	UserIdx = proto.useridx();
	CurrLocation = FVector(proto.x(), proto.y(), proto.z());
	CurrRotation = FRotator(proto.pitch(), 0.f, proto.yaw());
}

void Location::ConvertToProto(ProtoObject::Location& proto) const
{
	proto.set_useridx(UserIdx);
	proto.set_x(CurrLocation.X);
	proto.set_y(CurrLocation.Y);
	proto.set_z(CurrLocation.Z);
	proto.set_pitch(CurrRotation.Pitch);
	proto.set_yaw(CurrRotation.Yaw);
}

void RepBoolean::ConvertFromProto(const ProtoObject::RepBoolean& proto)
{
	UserIdx = proto.useridx();
	
	switch (proto.type())
	{
	case ProtoObject::RepBoolean_RepType_RUNNING:
		Type = RepType::RUNNING;
		break;
	case ProtoObject::RepBoolean_RepType_JUMPING:
		Type = RepType::JUMPING;
		break;
	case ProtoObject::RepBoolean_RepType_EQUIPPPED:
		Type = RepType::EQUIPPED;
		break;
	default:
		break;
	}

	Boolean = proto.boolean();
}

void RepBoolean::ConvertToProto(ProtoObject::RepBoolean& proto) const
{
	proto.set_useridx(UserIdx);

	switch (Type)
	{
	case RepType::RUNNING:
		proto.set_type(ProtoObject::RepBoolean_RepType_RUNNING);
		break;
	case RepType::JUMPING:
		proto.set_type(ProtoObject::RepBoolean_RepType_JUMPING);
		break;
	case RepType::EQUIPPED:
		proto.set_type(ProtoObject::RepBoolean_RepType_EQUIPPPED);
		break;
	default:
		break;
	}

	proto.set_boolean(Boolean);
}

void Equipment::ConvertFromProto(const ProtoObject::Equipment& proto)
{
	UserIdx = proto.useridx();

	switch (proto.equiptype())
	{
	case ProtoObject::Equipment_EquipmentType_CLOTH:
		EquipType = EquipmentType::CLOTH;
		break;
	case ProtoObject::Equipment_EquipmentType_WEAPON:
		EquipType = EquipmentType::WEAPON;
		break;
	default:
		break;
	}

	ItemIndex = proto.itemindex();
}

void Equipment::ConvertToProto(ProtoObject::Equipment& proto) const
{
	proto.set_useridx(UserIdx);
	
	switch(EquipType)
	{
	case EquipmentType::CLOTH:
		proto.set_equiptype(ProtoObject::Equipment_EquipmentType_CLOTH);
		break;
	case EquipmentType::WEAPON:
		proto.set_equiptype(ProtoObject::Equipment_EquipmentType_WEAPON);
		break;
	default:
		break;
	}

	proto.set_itemindex(ItemIndex);
}

void ResourceChange::ConvertFromProto(const ProtoObject::ResourceChange& proto)
{
	MapIdx = proto.mapidx();

	switch (proto.restype())
	{
	case ProtoObject::ResourceChange_ResourceType_ENEMY:
		ResType = ResourceType::ENEMY;
		break;
	case ProtoObject::ResourceChange_ResourceType_ITEM:
		ResType = ResourceType::ITEM;
		break;
	default:
		break;
	}

	switch (proto.reschangetype())
	{
	case ProtoObject::ResourceChange_ChangeType_SPAWN:
		ResChangeType = ChangeType::SPAWN;
		break;
	case ProtoObject::ResourceChange_ChangeType_REMOVE:
		ResChangeType = ChangeType::REMOVE;
		break;
	default:
		break;
	}

	ResIdx = proto.residx();
}

void ResourceChange::ConvertToProto(ProtoObject::ResourceChange& proto) const
{
	proto.set_mapidx(MapIdx);

	switch (ResType)
	{
	case ResourceType::ENEMY:
		proto.set_restype(ProtoObject::ResourceChange_ResourceType_ENEMY);
		break;
	case ResourceType::ITEM:
		proto.set_restype(ProtoObject::ResourceChange_ResourceType_ITEM);
		break;
	default:
		break;
	}

	switch (ResChangeType)
	{
	case ChangeType::SPAWN:
		proto.set_reschangetype(ProtoObject::ResourceChange_ChangeType_SPAWN);
		break;
	case ChangeType::REMOVE:
		proto.set_reschangetype(ProtoObject::ResourceChange_ChangeType_REMOVE);
		break;
	default:
		break;
	}

	proto.set_residx(ResIdx);
}

void MapTransition::ConvertFromProto(const ProtoObject::MapTransition& proto)
{
	UserIdx = proto.useridx();
	MapIdx_Before = proto.before();
	MapIdx_After = proto.after();
}

void MapTransition::ConvertToProto(ProtoObject::MapTransition& proto) const
{
	proto.set_useridx(UserIdx);
	proto.set_before(MapIdx_Before);
	proto.set_after(MapIdx_After);
}
