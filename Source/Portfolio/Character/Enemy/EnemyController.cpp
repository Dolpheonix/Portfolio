#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "../../Base/LoadHelper.h"
#include "../EnemyCharacter.h"
#include "../PlayerCharacter.h"

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

	// 퍼셉션 Parameter는 Enemy Character에서 설정
	TObjectPtr<UAISenseConfig_Sight> sightSense = Cast<UAISenseConfig_Sight>(GetPerceptionComponent()->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
	sightSense->SightRadius = ownerEnemy->SightRadius;
	sightSense->LoseSightRadius = ownerEnemy->LoseSightRadius;
	sightSense->PeripheralVisionAngleDegrees = ownerEnemy->SightAngle;
	sightSense->DetectionByAffiliation.bDetectEnemies = true;
	sightSense->DetectionByAffiliation.bDetectFriendlies = false;
	sightSense->DetectionByAffiliation.bDetectNeutrals = false;
	GetPerceptionComponent()->RequestStimuliListenerUpdate();

	if (mBehaviorTree && mBlackboardData)
	{
		UBlackboardComponent* blackboard = Blackboard.Get(); // TEMP : TObjectPtr와 UseBlackboard의 호환성 문제
		UseBlackboard(mBlackboardData, blackboard);
		RunBehaviorTree(mBehaviorTree);

		// 시작 위치를 Pivot으로 설정
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
	// Hurt 경직 시에는 이전 State를 캐싱한 후, 경직이 끝나면 되돌려야 함
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

	// 플레이어가 감지되었을 때
	if (Stimulus.WasSuccessfullySensed() == true)
	{
		// Patrol 상태에서 감지됨 ==> 타이머 등록, 감지 상태 2초간 유지되면 Caution 상태로 전환
		if (mCurrState == EEnemyState::Patrol)
		{
			SetState(EEnemyState::Caution);
			GetWorld()->GetTimerManager().SetTimer(mTimer_CautionToDetected, this, &AEnemyController::CautionToDetected, 0.5f, false, 2.0f);
			GetBlackboardComponent()->SetValueAsObject(TEXT("Target Player"), SourceActor);
		}
		// Detected 상태에서 감지됨 ==> ReleaseTarget 타이머가 이미 작동한 상태이므로 해제
		else if(mCurrState == EEnemyState::Detected)
		{
			GetWorld()->GetTimerManager().ClearTimer(mTimer_ReleaseTarget);
		}
	}
	// 플레이어가 없어졌을 때
	else
	{
		// Caution 상태에서 사라짐 ==> 타이머 해제
		if (mCurrState == EEnemyState::Caution)
		{
			SetState(EEnemyState::Patrol);
			GetWorld()->GetTimerManager().ClearTimer(mTimer_CautionToDetected);
		}
		// Detected 상태에서 사라짐 ==> 타이머 등록, 5초간 유지되면 Patrol 상태로 전환
		else if (mCurrState == EEnemyState::Detected)
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

	// 타겟 플레이어를 해제
	GetBlackboardComponent()->SetValueAsObject(TEXT("Target Player"), nullptr);

	SetState(EEnemyState::Patrol);
}

void AEnemyController::SetState(EEnemyState s)
{
	mCurrState = s;

	GetBlackboardComponent()->SetValueAsEnum(TEXT("Current State"), static_cast<uint8>(mCurrState));

	TObjectPtr<AEnemyCharacter> ownerEnemy = Cast<AEnemyCharacter>(GetPawn());
	check(ownerEnemy);

	// 캐릭터에도 State 변화 적용
	ownerEnemy->UpdateState(s);
}


