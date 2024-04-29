#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "Item.generated.h"

class UStaticMesh;
class UTexture2D;
class USoundCue;

constexpr int NUM_INVENTORY_ROW = 4;
constexpr int NUM_INVENTORY_COL = 4;
constexpr int MAX_ITEM_PER_TYPE = 16;

/*
	EItemType : 아이템 타입
		- Cloth : 의상
		- Weapon : 무기,
		- Item : 기타 아이템
*/
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Cloth,
	Weapon,
	Item,
	Count,
};

/*
	 EWeaponType : 무기 타입 (타입에 따라 공격 매커니즘이 다름)
		- Fist : 주먹,
		- Rifle : 총
*/
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Fist,
	Rifle,
	Count,
};

/*
	EThumbnailType : 아이템 썸네일 타입 (인벤토리, 상점 UI 등에서 사용)
		- Normal : 일반 썸네일
		- Hovered : 마우스 호버 상태 썸네일
		- Selected : 선택/장착된 아이템용 썸네일
*/
UENUM(BlueprintType)
enum class EThumbnailType : uint8
{
	Normal,
	Hovered,
	Selected,
	Count,
};

/*
	WeaponInfo : 무기 정보 (타입, 데미지 등)
*/
struct WeaponInfo
{
	EWeaponType Type;	// 무기 타입
	float Damage;		// 무기 데미지
	FString Sound;		// 공격 시 재생할 사운드 레퍼런스명
	FString BulletBP;	// Rifle 타입일 때, 스폰할 총알 클래스 레퍼런스명

public:
	WeaponInfo();
	WeaponInfo(const WeaponInfo& rhs) = default;
	WeaponInfo& operator=(const WeaponInfo& rhs) = default;
	~WeaponInfo();
};

/*
	ItemInfo : 아이템 정보 (타입, 가격 등)
*/
struct ItemInfo
{
	int Index;			// 게임 데이터 상의 인덱스
	EItemType Type;		// 아이템 타입
	FString Name;		// 아이템 이름
	bool bSellable;		// 상점에 팔 수 있는 아이템인가?
	int Price;			// 상점 판매 가격
	
	TArray<UTexture2D*> Thumbnail;	// 썸네일 이미지

	FString ItemMesh;	// 게임에 표시되는 아이템 메시 레퍼런스

	union
	{
		WeaponInfo SubInfo_Weapon;	// Weapon 타입인 경우 ==> 무기 정보
	};

	bool LoadFromJson(const TSharedPtr<FJsonObject>& infoVal);
	static bool LoadInfoListFromJson(TArray<ItemInfo>& list);
public:
	ItemInfo();
	ItemInfo(const ItemInfo& rhs);
	ItemInfo& operator=(const ItemInfo& rhs);
	~ItemInfo();
};

/*
	FGameItem : 인벤토리 상에 표시되는 아이템 단위. 개수와 종류
*/
USTRUCT(Atomic, BlueprintType)
struct FGameItem
{
	GENERATED_USTRUCT_BODY()

	// ItemInfo의 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int InfoIndex;
	// 보유 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int Num;

	bool LoadFromJson(const TSharedPtr<FJsonObject>& itemVal);

public:
	FGameItem();
	FGameItem(int index, int num);
	FGameItem(const FGameItem& rhs) = default;
	FGameItem& operator=(const FGameItem& rhs) = default;
};

/*
	FTypeInventory : 타입별로 보관할 아이템 리스트
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
	FInventory : 타입 인벤토리를 배열로 가지고 있는 통합 인벤토리
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