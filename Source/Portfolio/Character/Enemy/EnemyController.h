#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UAISenseConfig_Sight;

/*
	EEnemyState : Enemy 캐릭터의 State
	- Patrol : 플레이어를 감지하지 못한 상태. 일정 범위 내를 돌아다님
	- Caution : 플레이어가 처음 감지된 상태.
	- Detected: 일정 시간동안 Caution 상태가 유지되면 Detected 상태로 전환됨.
	- Hurt : 데미지를 입은 상태. 일정 시간 동안 경직
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
	AEnemyController : 적 캐릭터에 적용될 컨트롤러의 기반 클래스
	주요 기능 : 시야 퍼셉션, Enemy State 변환 로직
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

	// Caution에서 Detected 상태로 변환
	void CautionToDetected();
	// 타겟 플레이어를 해제
	void ReleaseTarget();
	// Enemy State를 변환
	void SetState(EEnemyState s);

protected:
	// 실행할 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBehaviorTree> mBehaviorTree;
	// 실행할 블랙보드 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBlackboardData> mBlackboardData;
	// 시야 퍼셉션
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAISenseConfig_Sight> mSenseConfig_Sight;

private:
	// 현재 Enemy State
	EEnemyState mCurrState;

	FTimerHandle mTimer_CautionToDetected;
	FTimerHandle mTimer_ReleaseTarget;
};
