#include "ReplicatedController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "../../Base/LoadHelper.h"
#include "../NpcCharacter.h"

AReplicatedController::AReplicatedController(const FObjectInitializer& objectInitializer)
{
	mBehaviorTree = LoadHelper::C_LoadObjectFromPath<UBehaviorTree>(TEXT("/Game/Blueprint/Character/Npc/NpcBehaviorTree.NpcBehaviorTree"));
	mBlackboardData = LoadHelper::C_LoadObjectFromPath<UBlackboardData>(TEXT("/Game/Blueprint/Character/Npc/NpcBlackboard.NpcBlackboard"));
}

void AReplicatedController::BeginPlay()
{
	Super::BeginPlay();
}

void AReplicatedController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* blackboard = Blackboard.Get(); //TEMP: UseBlackboard()�� ���ڷ� UBlackboardComponent*& �� ����
	UseBlackboard(mBlackboardData, blackboard);
	RunBehaviorTree(mBehaviorTree);

	check(GetBlackboardComponent());

	// ���� ��ġ
	GetBlackboardComponent()->SetValueAsVector(TEXT("Pivot"), GetPawn()->GetActorLocation());
}

void AReplicatedController::Tick(float DeltaTime)
{
}
