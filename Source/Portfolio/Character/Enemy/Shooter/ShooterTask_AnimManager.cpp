#include "ShooterTask_AnimManager.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "../../../Base/LoadHelper.h"

UShooterTask_AnimManager::UShooterTask_AnimManager()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;

	// 부울 값 블랙보드 키만 선택할 수 있음
	Transition_Move_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_AnimManager, Transition_Move_Key));
	Transition_Idle_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_AnimManager, Transition_Idle_Key));
	Transition_Attack_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_AnimManager, Transition_Attack_Key));
	Transition_Return_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_AnimManager, Transition_Return_Key));
	Transition_Hurt_Key.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UShooterTask_AnimManager, Transition_Hurt_Key));
}

EBTNodeResult::Type UShooterTask_AnimManager::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<ACharacter> ownerCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (ownerCharacter)
	{
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Rifle/AS_ShootingRack.AS_ShootingRack")), true);
	}
	return EBTNodeResult::InProgress;
}

void UShooterTask_AnimManager::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	TObjectPtr<UBlackboardComponent> blackboardComp = OwnerComp.GetBlackboardComponent();
	TObjectPtr<ACharacter> ownerCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());

	// 각 부울값이 true로 변경되면 해당 애니메이션으로 전환한다.
	if (blackboardComp->GetValueAsBool(Transition_Idle_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Idle_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Rifle/AS_ShootingRack.AS_ShootingRack")), true);
	}
	else if (blackboardComp->GetValueAsBool(Transition_Move_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Move_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Rifle/AS_ForwardMovementInRack_2.AS_ForwardMovementInRack_2")), true);
	}
	else if (blackboardComp->GetValueAsBool(Transition_Attack_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Attack_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Rifle/AS_Shot.AS_Shot")), false);
	}
	else if (blackboardComp->GetValueAsBool(Transition_Return_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Return_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Rifle/AS_ForwardMovementInRack_2.AS_ForwardMovementInRack_2")), true);
	}
	else if (blackboardComp->GetValueAsBool(Transition_Hurt_Key.SelectedKeyName) == true)
	{
		blackboardComp->SetValueAsBool(Transition_Hurt_Key.SelectedKeyName, false);
		ownerCharacter->GetMesh()->PlayAnimation(LoadHelper::LoadObjectFromPath<UAnimSequence>(TEXT("/Game/Character/Mannequins/Animations/Manny/MM_Land.MM_Land")), false);
	}
}
