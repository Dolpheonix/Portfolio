#pragma once

#include "CoreMinimal.h"
#include "../Character/EnemyCharacter.h"
#include "Enemy.generated.h"


/*
	EEnemyType : 적 캐릭터의 타입 (실제로 AEnemyCharacter를 상속받은 새 적 캐릭터 클래스마다 여기 추가해야 함)
*/
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Shooter,
	Monster,
	Count,
};

/*
	FEnemyLabelInfo : 적 캐릭터에 붙는 라벨(태그) : ex) 인간족, 몬스터, ...
		- 게임 데이터로 로드됨
*/
USTRUCT(BlueprintType)
struct FEnemyLabelInfo
{
	GENERATED_USTRUCT_BODY()

	FString Name;	// 라벨 이름

public:
	FEnemyLabelInfo();
	FEnemyLabelInfo(const FEnemyLabelInfo& rhs);
	FEnemyLabelInfo& operator=(const FEnemyLabelInfo& rhs);
	~FEnemyLabelInfo();

public:
	bool LoadFromJson(const TSharedPtr<FJsonObject>& infoVal);

	static bool LoadInfoListFromJson(TArray<FEnemyLabelInfo>& outList);
};

/*
	적 캐릭터의 종류 (EEnemyType과 같은 방식으로 클래스마다 하나의 info가 존재해야 함)
*/
USTRUCT(BlueprintType)
struct FEnemyInfo
{
	GENERATED_USTRUCT_BODY()

	int Index;	// 게임 데이터 상의 인덱스
	
	FString Name;	// 이름 (ex : Shooter, Dragon, ...)
	
	TSubclassOf<AEnemyCharacter> EnemyClass;	// 실제 캐릭터 클래스

	int Hp;	// 최대 HP
	int Damage;	// 데미지

	TArray<int> Labels;	// 라벨 목록

public:
	FEnemyInfo();
	FEnemyInfo(const FEnemyInfo& rhs);
	FEnemyInfo& operator=(const FEnemyInfo& rhs);
	~FEnemyInfo();

public:
	bool LoadFromJson(const TSharedPtr<FJsonObject>& infoVal);
	
	static bool LoadInfoListFromJson(TArray<FEnemyInfo>& outList);

	// EEnemyType에 맞는 클래스 오브젝트를 반환
	static TSubclassOf<AEnemyCharacter> GetEnemyClassFromType(const EEnemyType type);
};