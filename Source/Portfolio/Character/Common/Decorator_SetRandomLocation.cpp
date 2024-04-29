#include "Decorator_SetRandomLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UDecorator_SetRandomLocation::UDecorator_SetRandomLocation() : Radius(100.f)
{
	bNotifyActivation = true;

	// 백터 블랙보드만 선택할 수 있음
	Pivot.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_SetRandomLocation, Pivot));
	Output.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_SetRandomLocation, Output));
}

void UDecorator_SetRandomLocation::OnNodeActivation(FBehaviorTreeSearchData& searchData)
{
	Super::OnNodeActivation(searchData);

	TObjectPtr<AAIController> controller = searchData.OwnerComp.GetAIOwner();
	TObjectPtr<UBlackboardComponent> blackboardComp = controller->GetBlackboardComponent();

	const FVector pivotLocation = blackboardComp->GetValueAsVector(Pivot.SelectedKeyName);

	// 현재 Owner 캐릭터의 위치
	const FVector currentLocation = controller->GetPawn()->GetActorLocation();

	int cnt = 0;
	while (cnt < 10000)
	{
		FNavLocation destination;
		UNavigationSystemV1* navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		ANavigationData* navData = Cast<ANavigationData>(navSys->GetNavDataForActor(*Cast<AActor>(searchData.OwnerComp.GetAIOwner()->GetPawn())));

		// Pivot에서부터 Radius만큼 떨어진 랜덤 지점(Destination)
		navSys->GetRandomReachablePointInRadius(pivotLocation, Radius, destination, navData);
		if (FVector::Dist(currentLocation, destination.Location) > Radius * 0.5f)	// 단, 현재 위치와 목표 위치가 일정 이상 떨어져있어야 함 (Radius의 반)
		{
			blackboardComp->SetValueAsVector(Output.SelectedKeyName, destination.Location);
			return;
		}
		
		++cnt;
	}
}
