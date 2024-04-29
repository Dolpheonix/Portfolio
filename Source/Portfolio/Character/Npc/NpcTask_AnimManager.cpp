#include "NpcTask_AnimManager.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "../../Base/LoadHelper.h"

UNpcTask_AnimManager::UNpcTask_AnimManager()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	// 부울값 블랙보드 키만 선택 가능
	Transition_Move_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNpcTask_AnimManager, Transition_Move_Key));
	Transition_Idle_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNpcTask_AnimManager, Transition_Idle_Key));
	Transition_Interact_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNpcTask_AnimManager, Transition_Interact_Key));
}

EBTNodeResult::Type UNpcTask_AnimManager::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<ACharacter> ownerCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if(ownerCharacter)
	{
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Manny/MM_Idle.MM_Idle")), true);
	}
	return EBTNodeResult::InProgress;
}

void UNpcTask_AnimManager::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	TObjectPtr<UBlackboardComponent> blackboardComp = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	TObjectPtr<ACharacter> ownerCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (blackboardComp->GetValueAsBool(Transition_Move_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Move_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Manny/MM_Walk_Fwd.MM_Walk_Fwd")), true);
	}
	else if (blackboardComp->GetValueAsBool(Transition_Idle_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Idle_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Manny/MM_Idle.MM_Idle")), true);
	}
	else if (blackboardComp->GetValueAsBool(Transition_Interact_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Interact_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Manny/MM_Idle.MM_Idle")), true);
	}
}