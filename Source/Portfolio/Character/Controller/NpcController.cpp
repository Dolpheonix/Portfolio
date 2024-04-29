#include "NpcController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "../../Base/LoadHelper.h"
#include "../NpcCharacter.h"

ANpcController::ANpcController(const FObjectInitializer& objectInitializer) :
				Super(objectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))	// 블루프린트의 Detour Crowd Component(두 AI가 서로 부딪히지 않게 이동 방향을 바꿈.)
{
	// 기본 설정 값
	mBehaviorTree	= LoadHelper::C_LoadObjectFromPath<UBehaviorTree>(TEXT("/Game/Blueprint/Character/Npc/NpcBehaviorTree.NpcBehaviorTree"));
	mBlackboardData = LoadHelper::C_LoadObjectFromPath<UBlackboardData>(TEXT("/Game/Blueprint/Character/Npc/NpcBlackboard.NpcBlackboard"));
}

void ANpcController::BeginPlay()
{
	Super::BeginPlay();
}

void ANpcController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* blackboard = Blackboard.Get(); //TEMP: UseBlackboard()가 인자로 UBlackboardComponent*& 를 받음
	UseBlackboard(mBlackboardData, blackboard);
	RunBehaviorTree(mBehaviorTree);

	check(GetBlackboardComponent());

	// 기준 위치
	GetBlackboardComponent()->SetValueAsVector(TEXT("Pivot"), GetPawn()->GetActorLocation());
}

void ANpcController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANpcController::Interact(TObjectPtr<AActor> player)
{
	GetBlackboardComponent()->SetValueAsObject(TEXT("Interact Target"), player);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsInteracting"), true);
}

void ANpcController::UnInteract()
{
	GetBlackboardComponent()->SetValueAsObject(TEXT("Interact Target"), nullptr);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsInteracting"), false);
}
