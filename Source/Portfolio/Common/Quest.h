#pragma once

#include "CoreMinimal.h"
#include "Dialogue.h"
#include "Item.h"
#include "Quest.generated.h"

class UParticleSystem;

// 서브퀘스트의 종류
UENUM(BlueprintType)
enum class ESubQuestType : uint8
{
	Arrival,	// 지정된 위치까지 도착하는 퀘스트
	Collect,	// 특정 아이템을 정해진 개수만큼 수집하는 퀘스트
	Hunt,		// 특정 몹을 정해진 개수만큼 처치하는 퀘스트
	Action,		// 위 세 종류 외에, 특수한 행동으로 완료 가능한 퀘스트
	Count,
};

// 메인퀘스트 종류
UENUM(BlueprintType)
enum class EQuestType : uint8
{
	Serial,		// 서브퀘스트들을 차례대로 완료해야 하는 퀘스트
	Parallel,	// 서브퀘스트를 동시에 진행할수 있는 퀘스트
	Count,
};

// 퀘스트의 진행도
UENUM(BlueprintType) 
enum class EQuestProgressType : uint8
{
	UnAvailable,	// 진행 불가능
	Available,		// 진행 가능
	InProgess,		// 진행 중
	Completable,	// 완료 가능
	Completed,		// 완료
	Count,
};

// 퀘스트의 보상 타입
UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
	Exp,	// 경험치
	Item,	// 아이템
	Money,	// 골드
	Count,
};

// 퀘스트 완료 시 지급되는 보상
USTRUCT(Atomic, BlueprintType)
struct FQuestReward
{
	GENERATED_USTRUCT_BODY()

	EQuestRewardType Type;	// 보상 타입
	
	// 타입별 부가정보
	union
	{
		int ExpValue;			// 경험치량
		FGameItem ItemValue;	// 아이템
		int MoneyValue;			// 골드량
	};

	// Json 오브젝트로부터 게임 데이터 로드
	bool LoadFromJson(const TSharedPtr<FJsonObject>& rewardValue);

public:
	FQuestReward();
	FQuestReward(const FQuestReward& rhs) = default;
	FQuestReward& operator=(const FQuestReward& rhs) = default;
};

// 서브 퀘스트
USTRUCT(Atomic, BlueprintType)
struct FSubQuest
{
	GENERATED_USTRUCT_BODY()

	ESubQuestType Type;		// 서브퀘 타입
	FString Explanation;	// 퀘스트 설명
	int MainQuestIndex;		// 부모 메인퀘의 인덱스

	// 서브퀘 타입별 부가정보
	union
	{
		// 도착 서브퀘
		struct ArrivalForm
		{
			int		MapIndex;		// 맵의 인덱스
			FVector Destination;	// 맵 상의 위치
			UParticleSystem* FX;	// 도착지점에 표시할 파티클
		} ArrivalInfo;
		// 수집 서브퀘
		struct CollectForm
		{
			int Index;	// 아이템 종류
			int Num;	// 아이템 개수
		} CollectInfo;
		// 사냥 서브퀘
		struct HuntForm
		{
			int Index;	// 몹 종류
			int Num;	// 몹 개수
		} HuntInfo;
		// 액션 서브퀘
		struct ActionForm
		{
			int Code;	// 서브퀘의 고유 인덱스. 전역에서 겹치면 안됨
		} ActionInfo;
	};

	// Json 오브젝트로부터 게임 데이터 로드
	bool LoadFromJson(const TSharedPtr<FJsonObject>& sqValue);

public:
	FSubQuest();
	FSubQuest(const FSubQuest& rhs) = default;
	FSubQuest& operator=(const FSubQuest& rhs) = default;
};

// 퀘스트
USTRUCT(Atomic, BlueprintType)
struct FQuest
{
	GENERATED_USTRUCT_BODY()

	FString Name;					// 퀘스트 제목
	FString Explanation;			// 퀘스트 설명
	EQuestType Type;				// 퀘스트 타입
	TArray<FSubQuest> SubQuests;	// 서브퀘 목록
	TArray<FQuestReward> Rewards;	// 보상 목록

	// Json 파일로부터 전체 퀘스트 정보 로드
	static bool LoadInfoListFromJson(TArray<FQuest>& list);
	// Json 오브젝트로부터 게임 데이터 로드
	bool LoadFromJson(const TSharedPtr<FJsonObject>& questValue);

public:
	FQuest();
	FQuest(const FQuest& rhs) = default;
	FQuest& operator=(const FQuest& rhs) = default;
};

// 서브퀘의 진행 상황을 관리하는 구조체
USTRUCT(Atomic, BlueprintType)
struct FSubQuestStatus
{
	GENERATED_USTRUCT_BODY()

	// 서브퀘 타입
	UPROPERTY(BlueprintReadWrite)
	ESubQuestType Type;
	// 시작된 상태인가?
	UPROPERTY(BlueprintReadWrite)
	bool bStarted;
	// 완료되었는가?
	UPROPERTY(BlueprintReadWrite)
	bool bCompleted;
	// 현재 진행 상황 (사냥/수집 서브퀘)
	UPROPERTY(BlueprintReadWrite)
	int CurrAmount;

public:
	FSubQuestStatus();
	FSubQuestStatus(const FSubQuestStatus& rhs) = default;
	FSubQuestStatus& operator=(const FSubQuestStatus& rhs) = default;
};

// 메인 퀘스트 진행상황
USTRUCT(Atomic, BlueprintType)
struct FQuestStatus
{
	GENERATED_USTRUCT_BODY()

	// 리스트 상 인덱스
	UPROPERTY(BlueprintReadWrite)
	int Index;
	// 메인퀘 타입
	UPROPERTY(BlueprintReadWrite)
	EQuestType Type;
	// 현재 진행 상황
	UPROPERTY(BlueprintReadWrite)
	EQuestProgressType CurrProgress;
	// 서브퀘 진행 상황
	UPROPERTY(BlueprintReadWrite)
	TArray<FSubQuestStatus> SubStatus;
	// 타입별 부가 정보
	union
	{
		// Serial 퀘스트 : 현재 진행중인 서브퀘 인덱스
		UPROPERTY(BlueprintReadWrite)
		int CurrPhase;
		// Parallel 퀘스트 : 현재까지 완료한 서브퀘 수
		UPROPERTY(BlueprintReadWrite)
		int Completed;
	};
public:
	FQuestStatus();
	FQuestStatus(const FQuestStatus& rhs) = default;
	FQuestStatus& operator=(const FQuestStatus& rhs) = default;

	// 서브퀘 진행도가 변화된 상태라면, 전체 진행도를 업데이트
	void UpdateProgress();
};