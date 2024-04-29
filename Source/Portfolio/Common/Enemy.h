#pragma once

#include "CoreMinimal.h"
#include "../Character/EnemyCharacter.h"
#include "Enemy.generated.h"


/*
	EEnemyType : �� ĳ������ Ÿ�� (������ AEnemyCharacter�� ��ӹ��� �� �� ĳ���� Ŭ�������� ���� �߰��ؾ� ��)
*/
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Shooter,
	Monster,
	Count,
};

/*
	FEnemyLabelInfo : �� ĳ���Ϳ� �ٴ� ��(�±�) : ex) �ΰ���, ����, ...
		- ���� �����ͷ� �ε��
*/
USTRUCT(BlueprintType)
struct FEnemyLabelInfo
{
	GENERATED_USTRUCT_BODY()

	FString Name;	// �� �̸�

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
	�� ĳ������ ���� (EEnemyType�� ���� ������� Ŭ�������� �ϳ��� info�� �����ؾ� ��)
*/
USTRUCT(BlueprintType)
struct FEnemyInfo
{
	GENERATED_USTRUCT_BODY()

	int Index;	// ���� ������ ���� �ε���
	
	FString Name;	// �̸� (ex : Shooter, Dragon, ...)
	
	TSubclassOf<AEnemyCharacter> EnemyClass;	// ���� ĳ���� Ŭ����

	int Hp;	// �ִ� HP
	int Damage;	// ������

	TArray<int> Labels;	// �� ���

public:
	FEnemyInfo();
	FEnemyInfo(const FEnemyInfo& rhs);
	FEnemyInfo& operator=(const FEnemyInfo& rhs);
	~FEnemyInfo();

public:
	bool LoadFromJson(const TSharedPtr<FJsonObject>& infoVal);
	
	static bool LoadInfoListFromJson(TArray<FEnemyInfo>& outList);

	// EEnemyType�� �´� Ŭ���� ������Ʈ�� ��ȯ
	static TSubclassOf<AEnemyCharacter> GetEnemyClassFromType(const EEnemyType type);
};