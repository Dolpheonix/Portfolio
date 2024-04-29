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

	// Enemy 오브젝트에서 설정한 퍼셉션 인자 적용
	TObjectPtr<UAISenseConfig_Sight> sightSense = Cast<UAISenseConfig_Sight>(GetPerceptionComponent()->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
	sightSense->SightRadius = ownerEnemy->SightRadius;
	sightSense->LoseSightRadius = ownerEnemy->LoseSightRadius;
	sightSense->PeripheralVisionAngleDegrees = ownerEnemy->SightAngle;
	sightSense->DetectionByAffiliation.bDetectEnemies = true;
	sightSense->DetectionByAffiliation.bDetectFriendlies = false;
	sightSense->DetectionByAffiliation.bDetectNeutrals = false;
	GetPerceptionComponent()->RequestStimuliListenerUpdate();

	// 파생 클래스에서 설정한 비헤이비어 트리와 블랙보드 실행
	if (mBehaviorTree && mBlackboardData)
	{
		UBlackboardComponent* blackboard = Blackboard.Get(); //TEMP: UseBlackboard()가 인자로 UBlackboardComponent*& 를 받음
		UseBlackboard(mBlackboardData, blackboard);
		RunBehaviorTree(mBehaviorTree);

		// 현재 위치를 블랙보드에 설정 (패트롤 시에 기준점으로 사용)
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
	// Hurt 상태가 끝나게 되면, 기존의 State로 돌아가야 하기 때문에 이전 State를 블랙보드에 설정해야 함
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
		if (mCurrState == EEnemyState::Patrol) // 플레이어를 처음 발견한 상황, 2초간 대기 후 Detected 모드로 전환
		{
			SetState(EEnemyState::Caution);
			GetWorld()->GetTimerManager().SetTimer(mTimer_CautionToDetected, this, &AEnemyController::CautionToDetected, 0.5f, false, 2.0f);
			GetBlackboardComponent()->SetValueAsObject(TEXT("Target Player"), SourceActor);
		}
		else if(mCurrState == EEnemyState::Detected)	// Detected 상태에서 플레이어가 시야에서 벗어났다가 다시 발견됨
		{
			GetWorld()->GetTimerManager().ClearTimer(mTimer_ReleaseTarget);
		}
	}
	else
	{
		if (mCurrState == EEnemyState::Caution)	// Caution 상태에서 플레이어가 시야에서 벗어남
		{
			SetState(EEnemyState::Patrol);
			GetWorld()->GetTimerManager().ClearTimer(mTimer_CautionToDetected);
		}
		else if (mCurrState == EEnemyState::Detected)	// Detected 상태에서 플레이어가 시야에서 벗어난 상황. 5초간 대기 후 Patrol 상태로 전환
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

	// 타겟을 삭제
	GetBlackboardComponent()->SetValueAsObject(TEXT("Target Player"), nullptr);

	SetState(EEnemyState::Patrol);
}

void AEnemyController::SetState(EEnemyState s)
{
	mCurrState = s;

	// 블랙보드에 State 설정
	GetBlackboardComponent()->SetValueAsEnum(TEXT("Current State"), static_cast<uint8>(mCurrState));

	TObjectPtr<AEnemyCharacter> ownerEnemy = Cast<AEnemyCharacter>(GetPawn());
	check(ownerEnemy);

	// 캐릭터에 State 변경을 알림
	ownerEnemy->UpdateState(s);
}


