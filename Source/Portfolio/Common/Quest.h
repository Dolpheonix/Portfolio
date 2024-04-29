#pragma once

#include "CoreMinimal.h"
#include "Dialogue.h"
#include "Item.h"
#include "Quest.generated.h"

class UParticleSystem;

UENUM(BlueprintType)
enum class ESubQuestType : uint8
{
	Arrival,
	Collect,
	Hunt,
	Action,
	Count,
};

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	Serial,
	Parallel,
	Count,
};

UENUM(BlueprintType) 
enum class EQuestProgressType : uint8
{
	UnAvailable,
	Available,
	InProgess,
	Completable,
	Completed,
	Count,
};

UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
	Exp,
	Item,
	Money,
	Count,
};

USTRUCT(Atomic, BlueprintType)
struct FQuestReward
{
	GENERATED_USTRUCT_BODY()

	EQuestRewardType Type;
	union
	{
		int ExpValue;
		FGameItem ItemValue;
		int MoneyValue;
	};

	bool LoadFromJson(const TSharedPtr<FJsonObject>& rewardValue);

public:
	FQuestReward();
	FQuestReward(const FQuestReward& rhs) = default;
	FQuestReward& operator=(const FQuestReward& rhs) = default;
};

USTRUCT(Atomic, BlueprintType)
struct FSubQuest
{
	GENERATED_USTRUCT_BODY()

	ESubQuestType Type;
	FString Explanation;
	int MainQuestIndex;

	union
	{
		struct ArrivalForm
		{
			int		MapIndex;
			FVector Destination;
			UParticleSystem* FX;
		} ArrivalInfo;

		struct CollectForm
		{
			int Index;
			int Num;
		} CollectInfo;

		struct HuntForm
		{
			int Index;
			int Num;
		} HuntInfo;

		struct ActionForm
		{
			int Code;
		} ActionInfo;
	};

	bool LoadFromJson(const TSharedPtr<FJsonObject>& sqValue);

public:
	FSubQuest();
	FSubQuest(const FSubQuest& rhs) = default;
	FSubQuest& operator=(const FSubQuest& rhs) = default;
};

USTRUCT(Atomic, BlueprintType)
struct FQuest
{
	GENERATED_USTRUCT_BODY()

	FString Name;
	FString Explanation;
	EQuestType Type;
	TArray<FSubQuest> SubQuests;
	TArray<FQuestReward> Rewards;

	static bool LoadInfoListFromJson(TArray<FQuest>& list);
	bool LoadFromJson(const TSharedPtr<FJsonObject>& questValue);

public:
	FQuest();
	FQuest(const FQuest& rhs) = default;
	FQuest& operator=(const FQuest& rhs) = default;
};

USTRUCT(Atomic, BlueprintType)
struct FSubQuestStatus
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	ESubQuestType Type;
	UPROPERTY(BlueprintReadWrite)
	bool bStarted;
	UPROPERTY(BlueprintReadWrite)
	bool bCompleted;
	UPROPERTY(BlueprintReadWrite)
	int CurrAmount;

public:
	FSubQuestStatus();
	FSubQuestStatus(const FSubQuestStatus& rhs) = default;
	FSubQuestStatus& operator=(const FSubQuestStatus& rhs) = default;
};

USTRUCT(Atomic, BlueprintType)
struct FQuestStatus
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	int Index;
	UPROPERTY(BlueprintReadWrite)
	EQuestType Type;
	UPROPERTY(BlueprintReadWrite)
	EQuestProgressType CurrProgress;
	UPROPERTY(BlueprintReadWrite)
	TArray<FSubQuestStatus> SubStatus;

	union
	{
		UPROPERTY(BlueprintReadWrite)
		int CurrPhase;
		UPROPERTY(BlueprintReadWrite)
		int Completed;
	};
public:
	FQuestStatus();
	FQuestStatus(const FQuestStatus& rhs) = default;
	FQuestStatus& operator=(const FQuestStatus& rhs) = default;

	void UpdateProgress();
};