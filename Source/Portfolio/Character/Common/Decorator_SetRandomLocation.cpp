#include "Decorator_SetRandomLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UDecorator_SetRandomLocation::UDecorator_SetRandomLocation() : Radius(100.f)
{
	bNotifyActivation = true;

	Pivot.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_SetRandomLocation, Pivot));
	Output.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_SetRandomLocation, Output));
}

void UDecorator_SetRandomLocation::OnNodeActivation(FBehaviorTreeSearchData& searchData)
{
	Super::OnNodeActivation(searchData);

	TObjectPtr<AAIController> controller = searchData.OwnerComp.GetAIOwner();
	TObjectPtr<UBlackboardComponent> blackboardComp = controller->GetBlackboardComponent();

	const FVector pivotLocation = blackboardComp->GetValueAsVector(Pivot.SelectedKeyName);

	// 캐릭터의 현재 위치
	const FVector currentLocation = controller->GetPawn()->GetActorLocation();

	int cnt = 0;
	while (cnt < 10000)
	{
		FNavLocation destination;
		UNavigationSystemV1* navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		ANavigationData* navData = Cast<ANavigationData>(navSys->GetNavDataForActor(*Cast<AActor>(searchData.OwnerComp.GetAIOwner()->GetPawn())));

		// 랜덤 위치 설정
		navSys->GetRandomReachablePointInRadius(pivotLocation, Radius, destination, navData);
		// 현재 위치와 너무 가까우면 다시 설정
		if (FVector::Dist(currentLocation, destination.Location) > Radius * 0.5f)
		{
			blackboardComp->SetValueAsVector(Output.SelectedKeyName, destination.Location);
			return;
		}
		
		++cnt;
	}
}
