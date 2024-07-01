#pragma once

#include "CoreMinimal.h"
#include "../Network/ProtoObject/ProtoObject.pb.h"

/*
	gRPC를 사용하기 위해 Location, MapTransition 등의 송수신 구조체를 ProtoObject를 사용함.
	MainClient 외부에서는 이를 사용하지 않기 위해, 변수 값만을 담은 경량화 구조체를 따로 정의함
	여기 정의되는 구조체는 ProtoBuf 버전 클래스와의 상호 변경 메서드를 정의해야 함.
*/
namespace GameObject
{
	// 위치 변화 전송용 구조체
	struct Location
	{
		uint64 UserIdx;			// 유저 고유 번호
		FVector CurrLocation;	// 현재 위치	
		FRotator CurrRotation;	// 현재 회전 상태

	public:
		// Proto 객체로부터 값 복사
		void ConvertFromProto(const ProtoObject::Location& proto);
		// Proto 객체로 변환
		void ConvertToProto(ProtoObject::Location& proto) const;
	};
	
	// 애니메이션 전환용 Boolean 값을 담은 구조체
	struct RepBoolean
	{
		uint64 UserIdx;	// 유저 고유 번호
		
		// 변환할 애니메이션 타입
		enum class RepType : uint8
		{
			RUNNING,	// 달리기/걷기
			JUMPING,	// 점프
			EQUIPPED,	// 무기 장착
			COUNT,
		} Type;


		bool Boolean;	// 변환할 부울값
	
	public:
		void ConvertFromProto(const ProtoObject::RepBoolean& proto);
		void ConvertToProto(ProtoObject::RepBoolean& proto) const;
	};

	// 장착 무기/의상이 변경됨을 알리는 구조체
	struct Equipment
	{
		uint64 UserIdx;	// 유저 고유 번호

		// 장착한 아이템 타입
		enum class EquipmentType : uint8
		{
			CLOTH,	// 의상
			WEAPON,	// 무기
			COUNT,
		} EquipType;

		uint64 ItemIndex;	// ItemInfo 상의 인덱스

	public:
		void ConvertFromProto(const ProtoObject::Equipment& proto);
		void ConvertToProto(ProtoObject::Equipment& proto) const;
	};

	// 맵 상의 리소스가 변화했음을 알리는 구조체
	struct ResourceChange
	{
		uint32 MapIdx;	// 맵 인덱스

		// 변경된 리소스 타입
		enum class ResourceType : uint8
		{
			ENEMY,	// 몹
			ITEM,	// 루팅 아이템
			COUNT,
		} ResType;

		// 어떤 변화가 일어났는가?
		enum class ChangeType : uint8
		{
			REMOVE,	// 제거됨
			SPAWN,	// 스폰됨
			COUNT,
		} ResChangeType;

		// 리소스 인덱스
		uint64 ResIdx;

	public:
		void ConvertFromProto(const ProtoObject::ResourceChange& proto);
		void ConvertToProto(ProtoObject::ResourceChange& proto) const;
	};

	// 특정 유저의 맵 진입/이탈을 알리는 구조체
	struct MapTransition
	{
		uint64 UserIdx;			// 유저 고유 번호
		uint32 MapIdx_Before;	// 이탈하는 맵 인덱스
		uint32 MapIdx_After;	// 진입하는 맵 인덱스

	public:
		void ConvertFromProto(const ProtoObject::MapTransition& proto);
		void ConvertToProto(ProtoObject::MapTransition& proto) const;
	};
}