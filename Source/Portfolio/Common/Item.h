#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

class UStaticMesh;
class UTexture2D;
class USoundCue;

constexpr int NUM_INVENTORY_ROW = 4;	// 인벤토리, 상점 UI에서 아이템을 보여줄 때 가로에 배치되는 아이템 수
constexpr int NUM_INVENTORY_COL = 4;	// 세로에 배치되는 아이템 수
constexpr int MAX_ITEM_PER_TYPE = 16;	// 타입별로 최대 보유 가능한 아이템 수

/*
	아이템의 종류
*/
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Cloth,	// 의상
	Weapon,	// 무기
	Item,	// 기타 아이템
	Count,
};

/*
	무기 아이템의 타입 (공격 방식을 다르게 정의해야 함)
*/
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Fist,	// 주먹 (무기 없음)
	Rifle,	// 총기류
	Count,
};

/*
	아이템의 선택 여부에 따라 보여줄 썸네일 종류
*/
UENUM(BlueprintType)
enum class EThumbnailType : uint8
{
	Normal,		// 일반 썸네일
	Hovered,	// 마우스 호버 시 썸네일
	Selected,	// 선택된 아이템일 시 썸네일
	Count,
};

/*
	게임 데이터 상의 무기 정보
*/
struct WeaponInfo
{
	EWeaponType Type;	// 무기 종류
	float Damage;		// 무기 데미지
	FString Sound;		// 무기 효과음 (EX : 발사음)
	FString BulletBP;	// 총기류에 한해, 발사할 총알 클래스의 이름

public:
	WeaponInfo();
	WeaponInfo(const WeaponInfo& rhs) = default;
	WeaponInfo& operator=(const WeaponInfo& rhs) = default;
	~WeaponInfo();
};

/*
	게임 데이터 상의 아이템 정보
*/
struct ItemInfo
{
	int Index;			// 리스트 상 고유 인덱스
	EItemType Type;		// 아이템 종류
	FString Name;		// 아이템 표시명
	bool bSellable;		// 팔 수 있는 아이템인가?
	int Price;			// 아이템 가격
	
	TArray<UTexture2D*> Thumbnail;	// 썸네일 종류별 이미지

	FString ItemMesh;	// 인게임 상으로 보여줄 아이템에 한해, 아이템 메시 파일명

	// 아이템 종류별 부가정보
	union
	{
		WeaponInfo SubInfo_Weapon;	// 무기 아이템 정보
	};

	// Json 오브젝트로부터 게임 데이터 로드
	bool LoadFromJson(const TSharedPtr<FJsonObject>& infoVal);
	// Json 파일로부터 ItemInfo 전체 로드
	static bool LoadInfoListFromJson(TArray<ItemInfo>& list);
public:
	ItemInfo();
	ItemInfo(const ItemInfo& rhs);
	ItemInfo& operator=(const ItemInfo& rhs);
	~ItemInfo();
};

/*
	인게임 상으로 주고 받을 수 있는 아이템 구조체
*/
USTRUCT(Atomic, BlueprintType)
struct FGameItem
{
	GENERATED_USTRUCT_BODY()

	// ItemInfo 상 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int InfoIndex;
	// 아이템 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int Num;

	// Json 파일로부터 게임 데이터 로드
	bool LoadFromJson(const TSharedPtr<FJsonObject>& itemVal);

public:
	FGameItem();
	FGameItem(int index, int num);
	FGameItem(const FGameItem& rhs) = default;
	FGameItem& operator=(const FGameItem& rhs) = default;
};

/*
	같은 종류의 아이템만 모아놓은 리스트
	- WHY? : TArray를 2차원 배열로 사용할 수 없어서 Wrapper 구조체를 추가함
*/
USTRUCT(Atomic, BlueprintType)
struct FTypeInventory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FGameItem> ItemList;

public:
	FTypeInventory();
	FTypeInventory(const FTypeInventory& rhs) = default;
	FTypeInventory& operator=(const FTypeInventory& rhs) = default;
};

/*
	통합 인벤토리
*/
USTRUCT(Atomic, BlueprintType)
struct FInventory
{
	GENERATED_USTRUCT_BODY()

	FTypeInventory& GetTypeInventory(EItemType type);
	const FTypeInventory& GetTypeInventory(EItemType type) const;

	UPROPERTY(BlueprintReadWrite)
	TArray<FTypeInventory> TypeInventoryList;

public:
	FInventory();
	FInventory(const FInventory& rhs) = default;
	FInventory& operator=(const FInventory& rhs) = default;
};