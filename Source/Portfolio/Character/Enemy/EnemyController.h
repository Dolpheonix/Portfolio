#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UAISenseConfig_Sight;

/*
	EEnemyState : Enemy 캐릭터의 State
	- Patrol : 플레이어를 감지하지 못한 상태
	- Caution : 플레이어를 처음 감지한 상태
	- Detected: Caution 상태에서 감지 상태가 몇초간 유지되었으면, Detected 상태로 전환. 플레이어를 공격
	- Hurt : 플레이어에게 공격받아 경직이 걸린 상태
*/

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol,
	Caution,
	Detected,
	Hurt,
	Count,
};

/*
	몹 캐릭터용 컨트롤러
	- Enemy State 전환
	- 퍼셉션 등록, 관리
*/

UCLASS()
class PORTFOLIO_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyController(const FObjectInitializer& objectIntializer);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	void OnHurt();

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* SourceActor, FAIStimulus Stimulus);

	// Caution -> Detect
	void CautionToDetected();
	// Detect -> Patrol
	void ReleaseTarget();

	void SetState(EEnemyState s);

protected:
	// 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBehaviorTree> mBehaviorTree;
	// 블랙보드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBlackboardData> mBlackboardData;
	// 시각 퍼셉션
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAISenseConfig_Sight> mSenseConfig_Sight;

private:
	// 현재 State
	EEnemyState mCurrState;
	// Caution 상태에 진입하면 작동하는 타이머. 감지에서 벗어나면 해제.
	FTimerHandle mTimer_CautionToDetected;
	// Detect 상태에서 타겟이 감지를 벗어나면 작동하는 타이머. 다시 감지되면 해제/ 
	FTimerHandle mTimer_ReleaseTarget;
};
