#include "Item.h"
#include <cassert>
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "Sound/SoundCue.h"
#include "../Base/Func.h"
#include "../Base/LoadHelper.h"

static_assert(static_cast<uint8>(EItemType::Count) == 3);
static_assert(static_cast<uint8>(EWeaponType::Count) == 2);
static_assert(static_cast<uint8>(EThumbnailType::Count) == 3);

template<>
EItemType StringToEnum<EItemType>(const FString& str)
{
	if (str == "Cloth")
	{
		return EItemType::Cloth;
	}
	else if (str == "Weapon")
	{
		return EItemType::Weapon;
	}
	else if (str == "Item")
	{
		return EItemType::Item;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("EItemType has no member[%s]"), *str);
	}

	return EItemType::Count;
}

template<>
FString EnumToString<EItemType>(const EItemType& e)
{
	switch (e)
	{
	case EItemType::Cloth:
		return "Cloth";
	case EItemType::Weapon:
		return "Weapon";
	case EItemType::Item:
		return "Item";
	default:
	{
		UE_LOG(LogTemp, Fatal, TEXT("EItemType is invalid"));
		break;
	}
	}

	return "";
}

template<>
EWeaponType StringToEnum<EWeaponType>(const FString& str)
{
	if (str == "Fist")
	{
		return EWeaponType::Fist;
	}
	else if (str == "Rifle")
	{
		return EWeaponType::Rifle;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("EWeaponType has no member [%s]"), *str);
	}

	return EWeaponType::Count;
}

template<>
FString EnumToString<EWeaponType>(const EWeaponType& e)
{
	switch (e)
	{
	case EWeaponType::Fist:
		return "Fist";
	case EWeaponType::Rifle:
		return "Rifle";
	default:
	{
		UE_LOG(LogTemp, Fatal, TEXT("EWeaponType is invalid"));
		break;
	}
	}

	return "";
}

WeaponInfo::WeaponInfo() : Type(EWeaponType::Count), Damage(0.f), Sound(""), BulletBP("")
{
}

WeaponInfo::~WeaponInfo()
{
}

ItemInfo::ItemInfo() : Index(-1), Type(EItemType::Count), Name(""), bSellable(true), Price(0), ItemMesh(""), SubInfo_Weapon()
{
	Thumbnail.SetNum(static_cast<uint8>(EThumbnailType::Count));
}

ItemInfo::ItemInfo(const ItemInfo& rhs)
{
	Index = rhs.Index;
	Type = rhs.Type;
	Name = rhs.Name;
	bSellable = rhs.bSellable;
	Price = rhs.Price;
	ItemMesh = rhs.ItemMesh;
	SubInfo_Weapon = rhs.SubInfo_Weapon;
}

ItemInfo& ItemInfo::operator=(const ItemInfo& rhs)
{
	Index = rhs.Index;
	Type = rhs.Type;
	Name = rhs.Name;
	bSellable = rhs.bSellable;
	Price = rhs.Price;
	ItemMesh = rhs.ItemMesh;
	SubInfo_Weapon = rhs.SubInfo_Weapon;

	return *this;
}

ItemInfo::~ItemInfo()
{
}

bool ItemInfo::LoadFromJson(const TSharedPtr<FJsonObject>& infoVal)
{
	Index = infoVal->GetIntegerField("Index");
	Type = StringToEnum<EItemType>(infoVal->GetStringField("Type"));
	Name = infoVal->GetStringField("Name");
	bSellable = infoVal->GetBoolField("IsSellable");

	if (bSellable == true)
	{
		Price = infoVal->GetIntegerField("Price");
	}

	for (int j = 0; j < static_cast<uint8>(EThumbnailType::Count); ++j)
	{
		const FString& thumbnailName = infoVal->GetArrayField("Thumbnail")[j]->AsString();
		const FString thumbnailPath = LoadHelper::PATH_THUMBNAIL + thumbnailName;
		Thumbnail[j] = LoadHelper::LoadObjectFromPath<UTexture2D>(FName(*thumbnailPath));
	}

	if((Type == EItemType::Weapon) || (Type == EItemType::Cloth))
	{
		const FString& meshName = infoVal->GetStringField("Mesh");
		const FString meshPath = LoadHelper::PATH_MESH + meshName;
		ItemMesh = LoadHelper::PATH_MESH + infoVal->GetStringField("Mesh");
	}

	if (Type == EItemType::Weapon)
	{
		auto weaponObj = infoVal->GetObjectField("WeaponInfo");
		SubInfo_Weapon.Type = StringToEnum<EWeaponType>(weaponObj->GetStringField("Type"));
		SubInfo_Weapon.Damage = weaponObj->GetNumberField("Damage");
		SubInfo_Weapon.Sound = LoadHelper::PATH_SOUND + weaponObj->GetStringField("Sound");
		SubInfo_Weapon.BulletBP = LoadHelper::PATH_BLUEPRINT + "Actor/" + weaponObj->GetStringField("Bullet");
	}

	return true;
}

bool ItemInfo::LoadInfoListFromJson(TArray<ItemInfo>& list)
{
	FString result;
	const FString listPath = FPaths::ProjectContentDir() + "Data/ItemInfo.json";
	if (FFileHelper::LoadFileToString(result, *listPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("ItemInfo not loaded"));
		return false;
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	const TArray<TSharedPtr<FJsonValue>>& itemInfoList = root->GetArrayField(TEXT("ItemList"));
	list.Empty();
	list.SetNum(itemInfoList.Num());
	for (int i = 0; i < itemInfoList.Num(); ++i)
	{
		list[i].LoadFromJson(itemInfoList[i]->AsObject());
	}

	return true;
}

FGameItem::FGameItem() : InfoIndex(-1), Num(0) {}

FGameItem::FGameItem(int index, int num) : InfoIndex(index), Num(num) {}

bool FGameItem::LoadFromJson(const TSharedPtr<FJsonObject>& itemVal)
{
	check(itemVal);

	InfoIndex = itemVal->GetIntegerField("Index");
	Num		  = itemVal->GetIntegerField("Num");

	return true;
}

FTypeInventory::FTypeInventory() : ItemList() {}

FInventory::FInventory() 
{
	TypeInventoryList.SetNum(static_cast<uint8>(EItemType::Count));
}

FTypeInventory& FInventory::GetTypeInventory(EItemType type)
{
	return TypeInventoryList[static_cast<uint8>(type)];
}

const FTypeInventory& FInventory::GetTypeInventory(EItemType type) const
{
	return TypeInventoryList[static_cast<uint8>(type)];
}