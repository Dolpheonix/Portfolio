#include "NpcController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "../../Base/LoadHelper.h"
#include "../NpcCharacter.h"

ANpcController::ANpcController(const FObjectInitializer& objectInitializer) :
				Super(objectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))	// �������Ʈ�� Detour Crowd Component(�� AI�� ���� �ε����� �ʰ� �̵� ������ �ٲ�.)
{
	// 블랙보드, 비헤이비어 트리 설정
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

	UBlackboardComponent* blackboard = Blackboard.Get(); // TEMP : TObjectPtr과 UseBlackboard의 호환성 문제
	UseBlackboard(mBlackboardData, blackboard);
	RunBehaviorTree(mBehaviorTree);

	check(GetBlackboardComponent());

	// 시작 위치를 이동 Pivot으로 설정
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
