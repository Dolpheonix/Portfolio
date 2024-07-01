#include "ShooterController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "../../../Base/LoadHelper.h"

AShooterController::AShooterController(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	mBehaviorTree = LoadHelper::C_LoadObjectFromPath<UBehaviorTree>(TEXT("/Game/Blueprint/Character/Enemy/Shooter/ShooterBehaviorTree.ShooterBehaviorTree"));
	mBlackboardData = LoadHelper::C_LoadObjectFromPath<UBlackboardData>(TEXT("/Game/Blueprint/Character/Enemy/Shooter/ShooterBlackboard.ShooterBlackboard"));
}
