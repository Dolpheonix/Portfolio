#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UAISenseConfig_Sight;

/*
	EEnemyState : Enemy ĳ������ State
	- Patrol : �÷��̾ �������� ���� ����. ���� ���� ���� ���ƴٴ�
	- Caution : �÷��̾ ó�� ������ ����.
	- Detected: ���� �ð����� Caution ���°� �����Ǹ� Detected ���·� ��ȯ��.
	- Hurt : �������� ���� ����. ���� �ð� ���� ����
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
	AEnemyController : �� ĳ���Ϳ� ����� ��Ʈ�ѷ��� ��� Ŭ����
	�ֿ� ��� : �þ� �ۼ���, Enemy State ��ȯ ����
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

	// Caution���� Detected ���·� ��ȯ
	void CautionToDetected();
	// Ÿ�� �÷��̾ ����
	void ReleaseTarget();
	// Enemy State�� ��ȯ
	void SetState(EEnemyState s);

protected:
	// ������ �����̺�� Ʈ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBehaviorTree> mBehaviorTree;
	// ������ ������ ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TObjectPtr<UBlackboardData> mBlackboardData;
	// �þ� �ۼ���
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAISenseConfig_Sight> mSenseConfig_Sight;

private:
	// ���� Enemy State
	EEnemyState mCurrState;

	FTimerHandle mTimer_CautionToDetected;
	FTimerHandle mTimer_ReleaseTarget;
};
