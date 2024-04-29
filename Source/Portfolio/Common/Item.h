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
	EItemType : ������ Ÿ��
		- Cloth : �ǻ�
		- Weapon : ����,
		- Item : ��Ÿ ������
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
	 EWeaponType : ���� Ÿ�� (Ÿ�Կ� ���� ���� ��Ŀ������ �ٸ�)
		- Fist : �ָ�,
		- Rifle : ��
*/
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Fist,
	Rifle,
	Count,
};

/*
	EThumbnailType : ������ ����� Ÿ�� (�κ��丮, ���� UI ��� ���)
		- Normal : �Ϲ� �����
		- Hovered : ���콺 ȣ�� ���� �����
		- Selected : ����/������ �����ۿ� �����
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
	WeaponInfo : ���� ���� (Ÿ��, ������ ��)
*/
struct WeaponInfo
{
	EWeaponType Type;	// ���� Ÿ��
	float Damage;		// ���� ������
	FString Sound;		// ���� �� ����� ���� ���۷�����
	FString BulletBP;	// Rifle Ÿ���� ��, ������ �Ѿ� Ŭ���� ���۷�����

public:
	WeaponInfo();
	WeaponInfo(const WeaponInfo& rhs) = default;
	WeaponInfo& operator=(const WeaponInfo& rhs) = default;
	~WeaponInfo();
};

/*
	ItemInfo : ������ ���� (Ÿ��, ���� ��)
*/
struct ItemInfo
{
	int Index;			// ���� ������ ���� �ε���
	EItemType Type;		// ������ Ÿ��
	FString Name;		// ������ �̸�
	bool bSellable;		// ������ �� �� �ִ� �������ΰ�?
	int Price;			// ���� �Ǹ� ����
	
	TArray<UTexture2D*> Thumbnail;	// ����� �̹���

	FString ItemMesh;	// ���ӿ� ǥ�õǴ� ������ �޽� ���۷���

	union
	{
		WeaponInfo SubInfo_Weapon;	// Weapon Ÿ���� ��� ==> ���� ����
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
	FGameItem : �κ��丮 �� ǥ�õǴ� ������ ����. ������ ����
*/
USTRUCT(Atomic, BlueprintType)
struct FGameItem
{
	GENERATED_USTRUCT_BODY()

	// ItemInfo�� �ε���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	int InfoIndex;
	// ���� ����
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
	FTypeInventory : Ÿ�Ժ��� ������ ������ ����Ʈ
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
	FInventory : Ÿ�� �κ��丮�� �迭�� ������ �ִ� ���� �κ��丮
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