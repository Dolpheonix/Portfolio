#include "Quest.h"
#include "../Base/Func.h"
#include "../Base/LoadHelper.h"
#include "Particles/ParticleSystem.h"

static_assert(static_cast<uint8>(ESubQuestType::Count) == 4);
static_assert(static_cast<uint8>(EQuestType::Count) == 2);
static_assert(static_cast<uint8>(EQuestProgressType::Count) == 5);
static_assert(static_cast<uint8>(EQuestRewardType::Count) == 3);

template<>
ESubQuestType StringToEnum<ESubQuestType>(const FString& str)
{
	if (str == "Arrival")
	{
		return ESubQuestType::Arrival;
	}
	else if (str == "Collect")
	{
		return ESubQuestType::Collect;
	}
	else if (str == "Hunt")
	{
		return ESubQuestType::Hunt;
	}
	else if (str == "Action")
	{
		return ESubQuestType::Action;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid ESubQuestType [%s]"), *str);
	}

	return ESubQuestType::Count;
}

template<>
FString EnumToString<ESubQuestType>(const ESubQuestType& e)
{
	switch (e)
	{
	case ESubQuestType::Arrival:
		return "Arrival";
	case ESubQuestType::Collect:
		return "Collect";
	case ESubQuestType::Hunt:
		return "Hunt";
	case ESubQuestType::Action:
		return "Action";
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid ESubQuestType"));
		break;
	}

	return "";
}

template<>
EQuestType StringToEnum<EQuestType>(const FString& str)
{
	if (str == "Serial")
	{
		return EQuestType::Serial;
	}
	else if (str == "Parallel")
	{
		return EQuestType::Parallel;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EQuestType Value [%s]"), *str);
	}

	return EQuestType::Count;
}

template<>
FString EnumToString<EQuestType>(const EQuestType& e)
{
	switch (e)
	{
	case EQuestType::Serial:
		return "Serial";
	case EQuestType::Parallel:
		return "Parallel";
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EQuestType Value"));
		break;
	}

	return "";
}

template<>
EQuestProgressType StringToEnum<EQuestProgressType>(const FString& str)
{
	if (str == "UnAvailable")
	{
		return EQuestProgressType::UnAvailable;
	}
	else if (str == "Available")
	{
		return EQuestProgressType::Available;
	}
	else if (str == "InProgress")
	{
		return EQuestProgressType::InProgess;
	}
	else if (str == "Completable")
	{
		return EQuestProgressType::Completable;
	}
	else if (str == "Completed")
	{
		return EQuestProgressType::Completed;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EQuestProgressType Value [%s]"), *str);
	}

	return EQuestProgressType::Count;
}

template<>
FString EnumToString<EQuestProgressType>(const EQuestProgressType& e)
{
	switch (e)
	{
	case EQuestProgressType::UnAvailable:
		return "UnAvailable";
	case EQuestProgressType::Available:
		return "Available";
	case EQuestProgressType::InProgess:
		return "InProgress";
	case EQuestProgressType::Completable:
		return "Completable";
	case EQuestProgressType::Completed:
		return "Completed";
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EQuestProgressType Value"));
		break;
	}

	return "";
}

template<>
EQuestRewardType StringToEnum<EQuestRewardType>(const FString& str)
{
	if (str == "Exp")
	{
		return EQuestRewardType::Exp;
	}
	else if (str == "Item")
	{
		return EQuestRewardType::Item;
	}
	else if (str == "Money")
	{
		return EQuestRewardType::Money;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EQuestRewardType Value [%s]"), *str);
	}

	return EQuestRewardType::Count;
}

template<>
FString EnumToString<EQuestRewardType>(const EQuestRewardType& e)
{
	switch (e)
	{
	case EQuestRewardType::Exp:
		return "Exp";
	case EQuestRewardType::Item:
		return "Item";
	case EQuestRewardType::Money:
		return "Money";
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid EQuestRewardType Value"));
		break;
	}

	return "";
}

FQuestReward::FQuestReward() : Type(EQuestRewardType::Count), ExpValue(0)
{
}

bool FQuestReward::LoadFromJson(const TSharedPtr<FJsonObject>& rewardValue)
{
	Type = StringToEnum<EQuestRewardType>(rewardValue->GetStringField("Type"));
	switch (Type)
	{
	case EQuestRewardType::Exp:
		ExpValue = rewardValue->GetIntegerField("Exp");
		break;
	case EQuestRewardType::Item:
		ItemValue.LoadFromJson(rewardValue->GetObjectField("Item"));
		break;
	case EQuestRewardType::Money:
		MoneyValue = rewardValue->GetIntegerField("Money");
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Reward Type"));
		return false;
	}

	return true;
}

FSubQuest::FSubQuest() : Type(ESubQuestType::Count), Explanation(""), MainQuestIndex(-1), ArrivalInfo()
{
}

bool FSubQuest::LoadFromJson(const TSharedPtr<FJsonObject>& sqValue)
{
	Type = StringToEnum<ESubQuestType>(sqValue->GetStringField("Type"));
	Explanation = sqValue->GetStringField("Explanation");
	
	switch (Type)
	{
	case ESubQuestType::Arrival:
	{
		ArrivalInfo.MapIndex = sqValue->GetIntegerField("MapIndex");
		ArrivalInfo.Destination = Helper::JsonToVector(sqValue->GetArrayField("Destination"));

		const FString& particleName = sqValue->GetStringField("FX");
		const FString particlePath = LoadHelper::PATH_FX + particleName;
		ArrivalInfo.FX = LoadHelper::LoadObjectFromPath<UParticleSystem>(FName(*particlePath));
		break;
	}
	case ESubQuestType::Collect:
	case ESubQuestType::Hunt:
		CollectInfo.Index = sqValue->GetIntegerField("Index");
		CollectInfo.Num = sqValue->GetIntegerField("Num");
		break;
	case ESubQuestType::Action:
		ActionInfo.Code = sqValue->GetIntegerField("Code");
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid SubQuest Type"));
		return false;
	}

	return true;
}

FQuest::FQuest() : Name(""), Explanation(""), Type(EQuestType::Count), SubQuests(), Rewards()
{
}

bool FQuest::LoadInfoListFromJson(TArray<FQuest>& list)
{
	FString result;
	const FString listPath = FPaths::ProjectContentDir() + "Data/Quest.json";
	if (FFileHelper::LoadFileToString(result, *listPath) == false)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("QuestInfo not loaded"));
		return false;
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(result);
	TSharedPtr<FJsonObject> root = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(Reader, root);

	const TArray<TSharedPtr<FJsonValue>>& questInfoList = root->GetArrayField(TEXT("QuestList"));
	list.Empty();
	list.SetNum(questInfoList.Num());
	for (int i = 0; i < questInfoList.Num(); ++i)
	{
		list[i].LoadFromJson(questInfoList[i]->AsObject());
	}

	return true;
}

bool FQuest::LoadFromJson(const TSharedPtr<FJsonObject>& questValue)
{
	Name = questValue->GetStringField("Name");
	Explanation = questValue->GetStringField("Explanation");
	Type = StringToEnum<EQuestType>(questValue->GetStringField("Type"));

	const auto& sqValues = questValue->GetArrayField("SubQuests");
	SubQuests.SetNum(sqValues.Num());
	for (int i = 0; i < sqValues.Num(); ++i)
	{
		SubQuests[i].LoadFromJson(sqValues[i]->AsObject());
	}

	const auto& rewardValues = questValue->GetArrayField("Rewards");
	Rewards.SetNum(rewardValues.Num());
	for (int i = 0; i < rewardValues.Num(); ++i)
	{
		Rewards[i].LoadFromJson(rewardValues[i]->AsObject());
	}

	return true;
}

FSubQuestStatus::FSubQuestStatus() : Type(ESubQuestType::Count), bStarted(false), bCompleted(false), CurrAmount(0)
{
}

FQuestStatus::FQuestStatus() :  Index(-1), Type(EQuestType::Serial), CurrProgress(EQuestProgressType::Count), SubStatus(),
								CurrPhase(0)
{
}

void FQuestStatus::UpdateProgress()
{
	if (Type == EQuestType::Serial)
	{
		// 모든 서브퀘를 완료한 상황
		if (CurrPhase >= SubStatus.Num())
		{
			return;
		}
		// 현재 진행중인 서브퀘가 완료됨 --> CurrPhase를 증가시키고, 완료 가능 상태로 변경
		if (SubStatus[CurrPhase].bCompleted == true)
		{
			CurrPhase++;
			if (CurrPhase < SubStatus.Num())
			{
				SubStatus[CurrPhase].bStarted = true;
			}
			else
			{
				CurrProgress = EQuestProgressType::Completable;	// NPC와 대화를 통해 완료시킬 수 있음
			}
		}
	}
	else
	{
		// 완료한 서브퀘 수를 다시 체크
		Completed = 0;
		for (int i = 0; i < SubStatus.Num(); ++i)
		{
			if (SubStatus[i].bCompleted == true)
			{
				Completed++;
			}
		}

		if (Completed == SubStatus.Num())
		{
			CurrProgress = EQuestProgressType::Completable;
		}
		else
		{
			CurrProgress = EQuestProgressType::InProgess;
		}
	}
}
