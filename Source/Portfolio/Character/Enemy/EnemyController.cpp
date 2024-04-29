#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "../EnemyCharacter.h"
#include "../PlayerCharacter.h"
#include "../../Base/LoadHelper.h"

static_assert(static_cast<uint8>(EEnemyState::Count) == 4);

AEnemyController::AEnemyController(const FObjectInitializer& objectInitializer) :	Super(objectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowing"))),
																					mCurrState(EEnemyState::Patrol)
{
	PrimaryActorTick.bCanEverTick = true;

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception")));
	mSenseConfig_Sight = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Sense"));
	GetPerceptionComponent()->ConfigureSense(*mSenseConfig_Sight);
	GetPerceptionComponent()->SetDominantSense(*(mSenseConfig_Sight->GetSenseImplementation()));
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnTargetPerceptionUpdate);
	GetPerceptionComponent()->Activate();

	SetGenericTeamId(FGenericTeamId(2));
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TObjectPtr<AEnemyCharacter> ownerEnemy = Cast<AEnemyCharacter>(GetPawn());
	check(ownerEnemy);

	// Enemy ������Ʈ���� ������ �ۼ��� ���� ����
	TObjectPtr<UAISenseConfig_Sight> sightSense = Cast<UAISenseConfig_Sight>(GetPerceptionComponent()->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
	sightSense->SightRadius = ownerEnemy->SightRadius;
	sightSense->LoseSightRadius = ownerEnemy->LoseSightRadius;
	sightSense->PeripheralVisionAngleDegrees = ownerEnemy->SightAngle;
	sightSense->DetectionByAffiliation.bDetectEnemies = true;
	sightSense->DetectionByAffiliation.bDetectFriendlies = false;
	sightSense->DetectionByAffiliation.bDetectNeutrals = false;
	GetPerceptionComponent()->RequestStimuliListenerUpdate();

	// �Ļ� Ŭ�������� ������ �����̺�� Ʈ���� ������ ����
	if (mBehaviorTree && mBlackboardData)
	{
		UBlackboardComponent* blackboard = Blackboard.Get(); //TEMP: UseBlackboard()�� ���ڷ� UBlackboardComponent*& �� ����
		UseBlackboard(mBlackboardData, blackboard);
		RunBehaviorTree(mBehaviorTree);

		// ���� ��ġ�� �����忡 ���� (��Ʈ�� �ÿ� ���������� ���)
		GetBlackboardComponent()->SetValueAsVector(TEXT("Origin Location"), ownerEnemy->GetActorLocation());

		SetState(EEnemyState::Patrol);
	}
}

void AEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyController::OnHurt()
{
	// Hurt ���°� ������ �Ǹ�, ������ State�� ���ư��� �ϱ� ������ ���� State�� �����忡 �����ؾ� ��
	GetBlackboardComponent()->SetValueAsEnum(TEXT("Cached State"), static_cast<uint8>(mCurrState));
	SetState(EEnemyState::Hurt);
}

void AEnemyController::OnTargetPerceptionUpdate(AActor* SourceActor, FAIStimulus Stimulus)
{
	TObjectPtr<APlayerCharacter> targetPlayer = Cast<APlayerCharacter>(SourceActor);
	if (!targetPlayer)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed() == true)
	{
		if (mCurrState == EEnemyState::Patrol) // �÷��̾ ó�� �߰��� ��Ȳ, 2�ʰ� ��� �� Detected ���� ��ȯ
		{
			SetState(EEnemyState::Caution);
			GetWorld()->GetTimerManager().SetTimer(mTimer_CautionToDetected, this, &AEnemyController::CautionToDetected, 0.5f, false, 2.0f);
			GetBlackboardComponent()->SetValueAsObject(TEXT("Target Player"), SourceActor);
		}
		else if(mCurrState == EEnemyState::Detected)	// Detected ���¿��� �÷��̾ �þ߿��� ����ٰ� �ٽ� �߰ߵ�
		{
			GetWorld()->GetTimerManager().ClearTimer(mTimer_ReleaseTarget);
		}
	}
	else
	{
		if (mCurrState == EEnemyState::Caution)	// Caution ���¿��� �÷��̾ �þ߿��� ���
		{
			SetState(EEnemyState::Patrol);
			GetWorld()->GetTimerManager().ClearTimer(mTimer_CautionToDetected);
		}
		else if (mCurrState == EEnemyState::Detected)	// Detected ���¿��� �÷��̾ �þ߿��� ��� ��Ȳ. 5�ʰ� ��� �� Patrol ���·� ��ȯ
		{
			GetWorld()->GetTimerManager().SetTimer(mTimer_ReleaseTarget, this, &AEnemyController::ReleaseTarget, 0.2f, false, 5.0f);
		}
	}
}

void AEnemyController::CautionToDetected()
{
	GetWorld()->GetTimerManager().ClearTimer(mTimer_CautionToDetected);

	SetState(EEnemyState::Detected);
}

void AEnemyController::ReleaseTarget()
{
	GetWorld()->GetTimerManager().ClearTimer(mTimer_ReleaseTarget);

	// Ÿ���� ����
	GetBlackboardComponent()->SetValueAsObject(TEXT("Target Player"), nullptr);

	SetState(EEnemyState::Patrol);
}

void AEnemyController::SetState(EEnemyState s)
{
	mCurrState = s;

	// �����忡 State ����
	GetBlackboardComponent()->SetValueAsEnum(TEXT("Current State"), static_cast<uint8>(mCurrState));

	TObjectPtr<AEnemyCharacter> ownerEnemy = Cast<AEnemyCharacter>(GetPawn());
	check(ownerEnemy);

	// ĳ���Ϳ� State ������ �˸�
	ownerEnemy->UpdateState(s);
}


