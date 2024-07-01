#pragma once

#include "CoreMinimal.h"
#include "../Character/EnemyCharacter.h"
#include "Enemy.generated.h"


/*
	몹 타입 : 스폰할 액터의 클래스와 연결하는 용도
*/
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Shooter,	// 플레이어에게 총을 쏘는 몹
	Monster,	// 플레이어에게 달려드는 몹
	Count,
};

/*
	몹의 특성 (ex : 인간족, ...)
*/
USTRUCT(BlueprintType)
struct FEnemyLabelInfo
{
	GENERATED_USTRUCT_BODY()

	FString Name;	// 표시명

public:
	FEnemyLabelInfo();
	FEnemyLabelInfo(const FEnemyLabelInfo& rhs);
	FEnemyLabelInfo& operator=(const FEnemyLabelInfo& rhs);
	~FEnemyLabelInfo();

public:
	// Json 객체로부터 정보를 받아옴
	bool LoadFromJson(const TSharedPtr<FJsonObject>& infoVal);
	// 게임데이터를 로드해오는 함수. 게임 시작 시에 호출됨
	static bool LoadInfoListFromJson(TArray<FEnemyLabelInfo>& outList);
};

/*
	게임 데이터 상의 몹 정보
*/
USTRUCT(BlueprintType)
struct FEnemyInfo
{
	GENERATED_USTRUCT_BODY()

	int Index;		// 게임 데이터 상의 인덱스
	
	FString Name;	// 몹 이름(종류) (ex : Shooter, Dragon, ...)
	
	TSubclassOf<AEnemyCharacter> EnemyClass;	// 스폰 시 사용할 클래스

	int Hp;	// 최대 HP
	int Damage;	// 공격 데미지

	TArray<int> Labels;	// 몹이 가지는 특성 모음

public:
	FEnemyInfo();
	FEnemyInfo(const FEnemyInfo& rhs);
	FEnemyInfo& operator=(const FEnemyInfo& rhs);
	~FEnemyInfo();

public:
	// Json 객체로부터 정보를 받아옴
	bool LoadFromJson(const TSharedPtr<FJsonObject>& infoVal);
	// 게임데이터를 로드해오는 함수. 게임 시작 시에 호출됨
	static bool LoadInfoListFromJson(TArray<FEnemyInfo>& outList);

	// 몹의 타입으로부터 스폰할 액터 클래스를 불러옴
	static TSubclassOf<AEnemyCharacter> GetEnemyClassFromType(const EEnemyType type);
};