#include "Decorator_SetBoolean.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UDecorator_SetBoolean::UDecorator_SetBoolean() : BooleanValue(false)
{
	bNotifyActivation = true;

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_SetBoolean, BlackboardKey));
}

void UDecorator_SetBoolean::OnNodeActivation(FBehaviorTreeSearchData& searchData)
{
	searchData.OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, BooleanValue);
}

