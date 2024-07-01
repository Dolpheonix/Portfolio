#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "CustomCharacter.h"
#include "../Common/Item.h"
#include "ReplicatedCharacter.generated.h"

UENUM(BlueprintType)
enum class EReplicatedActionMode : uint8
{
	Idle,	// Idle
	Attack,	// 공격 중
	Count,
};

UENUM(BlueprintType)
enum class EReplicatedMovementMode : uint8
{
	Idle,	// Idle
	Walk,	// 걷는 중
	Run,	// 달리는 중
	Jump,	// 점프 중
	Fall,	// 낙하 중
	Count,
};

class UWeapon;

/*
	멀티플레이어의 움직임을 재현하는 캐릭터.
	- 이동, 점프, 낙하
	- 무기 장착 상태
*/
UCLASS()
class PORTFOLIO_API AReplicatedCharacter : public ACustomCharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	AReplicatedCharacter();
	virtual FGenericTeamId GetGenericTeamId() { return FGenericTeamId(2); };
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;

	void UpdateLocationAndRotation(const FVector& loc, const FRotator& rot);
	void ChangeWeapon(int infoIndex);

	void SetRunning(bool newval);
	void SetJumping(bool newval);
	void SetEquipping(bool newval);

	bool GetRunning() const;
	bool GetJumping() const;
	bool GetEquipping() const;

	void SetResourceIndex(const int idx);

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UWeapon> mWeapon;

	UPROPERTY(BlueprintReadWrite)
	EReplicatedActionMode mActionMode;

	UPROPERTY(BlueprintReadWrite)
	EReplicatedMovementMode mMovementMode;

	bool bRunning;
	bool bJumping;
	bool bEquipping;

	EWeaponType mWeaponType;

	uint8 mLoco_Action_Blend;

	uint64 mResIdx;
};
