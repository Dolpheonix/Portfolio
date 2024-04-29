#include "Enemy.h"
#include "../Base/Func.h"
#include "../Character/Enemy/Shooter.h"
#include "../Character/Enemy/Monster.h"

static_assert(static_cast<uint8>(EEnemyType::Count) == 2);

template<>
EEnemyType StringToEnum<EEnemyType>(const FString& str)
{
	if (str == "Shooter")
	{
		return EEnemyType::Shooter;
	}
	else if (str == "Monster")
	{
		return EEnemyType::Monster;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("EEnemyType has no member [%s]"), *str);
	}
	
	return EEnemyType::Count;
}

template<>
FString EnumToString<EEnemyType>(const EEnemyType& e)
{
	switch (e)
	{
	case EEnemyType::Shooter:
		return "Shooter";
	case EEnemyType::Monster:
		return "Monster";
	default:
	{
		UE_LOG(LogTemp, Fatal, TEXT("EEnemyType is invalid"));
		break;
	}
	}

	return "";
}

FEnemyLabelInfo::FEnemyLabelInfo() : Name("")
{
}

FEnemyLabelInfo::FEnemyLabelInfo(const FEnemyLabelInfo& rhs)
{
	Name = rhs.Name;
}

FEnemyLabelInfo& FEnemyLabelInfo::operator=(const FEnemyLabelInfo& rhs)
{
	Name = rhs.Name;
	
	return *this;
}

FEnemyLabelInfo::~FEnemyLabelInfo()
{
}

bool FEnemyLabelInfo::LoadFromJson(const TSharedPtr<FJsonObject>& infoVal)
{
	Name = infoVal->GetStringField("Name");

	return true;
}

bool FEnemyLabelInfo::LoadInfoListFromJson(TArray<FEnemyLabelInfo>& outList)
{
	FString result;
	const FString listPath = FPaths::ProjectContentDir() + "Data/EnemyLabelInfo.json";

	if (FFileHelper::LoadFileToString(result, *listPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("EnemyLabelInfo not loaded"));
		return false;
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	const TArray<TSharedPtr<FJsonValue>>& enemyLabelInfoList = root->GetArrayField("LabelList");
	outList.Empty();
	outList.SetNum(enemyLabelInfoList.Num());
	for (int i = 0; i < enemyLabelInfoList.Num(); ++i)
	{
		outList[i].LoadFromJson(enemyLabelInfoList[i]->AsObject());
	}

	return true;
}

FEnemyInfo::FEnemyInfo() : Index(-1), Name(""), EnemyClass(nullptr), Hp(0.f), Damage(0.f), Labels()
{
}

FEnemyInfo::FEnemyInfo(const FEnemyInfo& rhs)
{
	Index = rhs.Index;
	Name = rhs.Name;
	EnemyClass = rhs.EnemyClass;
	Hp = rhs.Hp;
	Damage = rhs.Damage;
	Labels = rhs.Labels;
}

FEnemyInfo& FEnemyInfo::operator=(const FEnemyInfo& rhs)
{
	Index = rhs.Index;
	Name = rhs.Name;
	EnemyClass = rhs.EnemyClass;
	Hp = rhs.Hp;
	Damage = rhs.Damage;
	Labels = rhs.Labels;

	return *this;
}

FEnemyInfo::~FEnemyInfo()
{
}

bool FEnemyInfo::LoadFromJson(const TSharedPtr<FJsonObject>& infoVal)
{
	Index = infoVal->GetIntegerField("Index");
	Name = infoVal->GetStringField("Name");
	Hp = infoVal->GetIntegerField("Hp");
	Damage = infoVal->GetIntegerField("Damage");
	
	const TArray<TSharedPtr<FJsonValue>>& labelArr = infoVal->GetArrayField("Labels");
	Labels.SetNum(labelArr.Num());
	for (int i = 0; i < labelArr.Num(); ++i)
	{
		Labels[i] = labelArr[i]->AsNumber();
	}

	EnemyClass = GetEnemyClassFromType(StringToEnum<EEnemyType>(infoVal->GetStringField("Class")));

	return true;
}

bool FEnemyInfo::LoadInfoListFromJson(TArray<FEnemyInfo>& outList)
{
	FString result;
	const FString listPath = FPaths::ProjectContentDir() + "Data/EnemyInfo.json";
	
	if (FFileHelper::LoadFileToString(result, *listPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("EnemyInfo not loaded"));
		return false;
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	const TArray<TSharedPtr<FJsonValue>>& enemyInfoList = root->GetArrayField("EnemyInfoList");
	outList.Empty();
	outList.SetNum(enemyInfoList.Num());
	for (int i = 0; i < enemyInfoList.Num(); ++i)
	{
		outList[i].LoadFromJson(enemyInfoList[i]->AsObject());
	}

	return true;
}

TSubclassOf<AEnemyCharacter> FEnemyInfo::GetEnemyClassFromType(const EEnemyType type)
{
	switch (type)
	{
	case EEnemyType::Shooter:
		return AShooter::StaticClass();
	case EEnemyType::Monster:
		return AMonster::StaticClass();
	default:
		return nullptr;
	}

	return nullptr;
}
